#include <google/protobuf/descriptor.h>
#include <google/protobuf/service.h>
#include <rpc/rpc_application.h>
#include <rpc/rpc_header.pb.h>
#include <rpc/rpc_provider.h>

namespace talko::rpc {
RpcProvider::RpcProvider(net::Duration enroll_timeout)
    : enroll_timeout_(enroll_timeout) {
}

void RpcProvider::publish(ServicePtr service) {
    ServiceInfo info;
    info.service = service;

    // 获取服务对象的描述信息
    ServiceDescriptorPtr service_desc = service->GetDescriptor();

    std::string service_name = service_desc->name();         // 服务名称
    int         method_cnt   = service_desc->method_count(); // 服务数量

    for (int i = 0; i < method_cnt; ++i) {
        MethodDescriptorPtr method_desc = service_desc->method(i);

        // 获取方法名称
        std::string method_name = method_desc->name();

        // 存储到服务映射表中
        info.methods[method_name] = method_desc;
    }

    services_[service_name] = info;
}

void RpcProvider::run() {
    // 设置服务器参数
    net::TcpServer server(&loop_, RpcApplication::instance().serverAddress(),
        RpcApplication::instance().serverName(), RpcApplication::instance().reusePort());

    // 设置子事件循环数量
    server.setSubLoopSize(RpcApplication::instance().subloopSize());

    // 设置相应的回调函数
    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::onMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3));

    LOGGER_DEBUG("rpc", "Try to enroll all methods");

    // 向注册中心注册当前RPC节点上所有的服务
    for (auto& [service_name, service_info] : services_) {
        for (auto& [method_name, _] : service_info.methods) {
            if (!RpcRegistrant::instance().enrollMethod(service_name, method_name, enroll_timeout_)) {
                LOGGER_FATAL("rpc", "Failed to enroll [{}]-[{}]: {}", service_name, method_name,
                    RpcRegistrant::instance().errorMessage());
            }
        }
    }

    LOGGER_INFO("rpc", "Finished to enroll all methods");

    server.start();
    LOGGER_INFO("rpc", "{} start at {}", server.name(), server.ipPort());

    loop_.loop();
}

void RpcProvider::onConnection(const net::TcpConnectionPtr& conn) {
    if (conn->disconnected()) {
        conn->shutdown();
    }
}

void RpcProvider::onMessage(const net::TcpConnectionPtr& conn,
    net::ByteBuffer* buffer, net::TimePoint time) {
    std::string message;
    buffer->readBytes(message);

    // 读取前4个字节作为头部序列大小
    uint32_t header_size = 0;
    message.copy(reinterpret_cast<char*>(&header_size), sizeof(header_size), 0);

    // 根据头部序列大小读取头部内容
    std::string rpc_header_content = message.substr(4, header_size);
    RpcHeader   rpc_header;

    std::string service_name, method_name;
    uint32_t    args_size;

    // 反序列化头部内容
    if (rpc_header.ParseFromString(rpc_header_content)) {
        // 数据头反序列化成功
        service_name = rpc_header.service_name(); // 服务名称
        method_name  = rpc_header.method_name();  // 方法名称
        args_size    = rpc_header.args_size();    // 参数大小
    } else {
        // 反序列化失败
        LOGGER_ERROR("rpc", "Failed to deserialize RpcHeader: {}", rpc_header_content);
        return;
    }

    // 剩下的内容即为参数内容
    std::string args_content = message.substr(4 + header_size);
    LOGGER_TRACE("rpc", "Deserialize result: ServiceName[{}] MethodName[{}] ArgsSize[{}] Args[{}]",
        service_name, method_name, args_size, args_content);

    auto srv_it = services_.find(service_name);
    if (srv_it == services_.end()) {
        LOGGER_ERROR("rpc", "Service[{}] is not exist", service_name);
        return;
    }

    // 获取服务对象
    ServicePtr service = srv_it->second.service;

    auto mtd_id = srv_it->second.methods.find(method_name);
    if (mtd_id == srv_it->second.methods.end()) {
        LOGGER_ERROR("rpc", "Method[{}] is not exist", method_name);
        return;
    }

    // 获取方法对象
    MethodDescriptorPtr method = mtd_id->second;

    // 反序列化远端RPC的请求数据
    MessagePtr request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_content)) {
        LOGGER_ERROR("rpc", "Failed to deserialize request Args: {}", args_content);
        return;
    }

    MessagePtr response = service->GetResponsePrototype(method).New();

    // 生成回调方法用于序列化响应数据并发送给远端的RPC服务请求方
    ClosurePtr closure = google::protobuf::NewCallback<RpcProvider, const net::TcpConnectionPtr&,
        MessagePtr>(this, &RpcProvider::sendRpcResponse, conn, response);

    // 根据远端RPC请求 调用当前RPC节点上发布的具体方法
    service->CallMethod(method, nullptr, request, response, closure);
}

void RpcProvider::sendRpcResponse(const net::TcpConnectionPtr& conn, MessagePtr response) {
    LOGGER_TRACE("rpc", "send rpc response");

    std::string content;
    if (response->SerializeToString(&content)) {
        conn->send(content);
    } else {
        LOGGER_ERROR("rpc", "Failed to serialize response");
    }

    conn->shutdown(); // 由RPC服务提供方法主动断开连接
}
} // namespace talko::rpc