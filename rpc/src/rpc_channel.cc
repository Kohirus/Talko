#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <rpc/rpc_application.h>
#include <rpc/rpc_channel.h>
#include <rpc/rpc_header.pb.h>

namespace talko::rpc {
RpcChannel::RpcChannel(net::Duration timeout)
    : client_(&loop_, rpc::RpcApplication::instance().serverAddress(), "RpcRequester")
    , timeout_(timeout) {
    client_.enableRetry();
    client_.setConnectionCallback(std::bind(&RpcChannel::onConnection, this, std::placeholders::_1));
    client_.setMessageCallback(std::bind(&RpcChannel::onMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3));
}

void RpcChannel::CallMethod(MethodDescriptorPtr method, RpcControllerPtr controller,
    ConstMessagePtr request, MessagePtr response, ClosurePtr done) {
    ServiceDescriptorPtr service = method->service();

    // 获取服务名称和方法名称
    std::string service_name = service->name();
    std::string method_name  = method->name();

    // 序列化RPC请求参数
    std::string args_content;
    if (!request->SerializeToString(&args_content)) {
        controller->SetFailed("Failed to serialize request");
        return;
    } else {
        LOGGER_TRACE("rpc", "Request serialization is complete");
    }

    // 获取RPC请求参数的大小
    size_t args_size = args_content.size();

    // 生成RPC请求头
    RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(static_cast<uint32_t>(args_size));

    // 序列化RPC请求头
    std::string header_content;
    if (!rpc_header.SerializeToString(&header_content)) {
        controller->SetFailed("Failed to serialize RpcHeader");
        return;
    } else {
        LOGGER_TRACE("rpc", "RpcHeader serialization is complete");
    }

    // 获取RPC请求头的大小
    uint32_t header_size = static_cast<uint32_t>(header_content.size());

    // 组织待发送的RPC内容
    // -----------------------------------------------------------------------------------------------
    // | Header Content Size(4) | Header Content(Service Name,Method Name,Args Size) | Request Args |
    // -----------------------------------------------------------------------------------------------
    package_.clear();
    package_.append(std::string(reinterpret_cast<const char*>(&header_size), 4));
    package_.append(header_content);
    package_.append(args_content);

    LOGGER_TRACE("rpc", "Packaged RpcHeader: HeaderSize[{}] HeaderContent[{}] ServiceName[{}] MethodName[{}] ArgsSize[{}] Args[{}]",
        header_size, header_content, service_name, method_name, args_size, args_content);
    result_.clear();

    // 建立TCP连接
    controller_ = controller;
    client_.connect();

    // 启动定时器 防止RPC调用超时 对于RPC调用方 可能的超时情况如下：
    // 1. 与服务端建立连接时导致的超时；
    // 2. 组织RPC请求数据导致的超时；(并未处理)
    // 3. 等待服务端处理RPC请求导致的超时；
    // 4. 接收到服务端RPC响应后由于解析响应数据导致的超时；(并未处理)
    loop_.runAfter(timeout_, std::bind(&RpcChannel::handleTimeout, this));
    loop_.loop();

    if (!controller->Failed()) {
        LOGGER_DEBUG("rpc", "RPC Request is complete");
        // 对接收到的RPC应答进行反序列化
        if (!response->ParseFromString(result_)) {
            controller->SetFailed("Failed to parse response data");
        } else {
            LOGGER_TRACE("rpc", "Response parsing is complete");
        }
    }

    controller_ = nullptr;
    if (done) done->Run();
}

void RpcChannel::onConnection(const net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        LOGGER_INFO("rpc", "Connection established with RpcProvider");
        conn->send(package_);
        LOGGER_TRACE("rpc", "Send package to RpcProvider: {}", package_);
    } else {
        LOGGER_INFO("rpc", "Connection destoryed with RpcProvider");
        conn->loop()->quit();
    }
}

void RpcChannel::onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time) {
    buffer->readBytes(result_);
    LOGGER_TRACE("rpc", "Receive response: {}", result_);
    client_.disconnect();
}

void RpcChannel::handleTimeout() {
    controller_->SetFailed("RPC request is timeout");
    controller_->StartCancel();
    client_.stop();
    client_.loop()->quit();
}
} // namespace talko::rpc