#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <rpc/rpc_application.h>
#include <rpc/rpc_channel.h>
#include <rpc/rpc_header.pb.h>
#include <rpc/rpc_regedit.pb.h>

namespace talko::rpc {
RpcChannel::RpcChannel(net::Duration discover_timeout)
    : discover_timeout_(discover_timeout) {
}

void RpcChannel::CallMethod(MethodDescriptorPtr method, RpcControllerPtr controller,
    ConstMessagePtr request, MessagePtr response, ClosurePtr done) {
    LOGGER_TRACE("rpc", "Call method to get response of service");
    ServiceDescriptorPtr service = method->service();

    // 获取服务名称和方法名称
    std::string service_name = service->name();
    std::string method_name  = method->name();

    net::TimePoint start_time = std::chrono::high_resolution_clock::now();

    net::InetAddress service_addr;
    if (!RpcRegistrant::instance().discoverMethod(service_name, method_name, discover_timeout_, service_addr)) {
        controller->SetFailed(RpcRegistrant::instance().errorMessage());
        if (done) done->Run();
        return;
    }

    LOGGER_INFO("rpc", "[{}]-[{}] is located on {}", service_name, method_name, service_addr.toIpPort());

    // 组织请求数据
    createRequestContent(service_name, method_name, controller, request);
    if (controller->Failed()) {
        if (done) done->Run();
        return;
    }

    net::TimePoint end_time = std::chrono::high_resolution_clock::now();

    // 计算剩余的超时时间
    remaining_timeout_ = discover_timeout_ - std::chrono::duration_cast<net::Duration>(end_time - start_time);
    if (remaining_timeout_.count() <= 0) {
        controller->SetFailed("CallMethod timeout");
        if (done) done->Run();
        return;
    }

    LOGGER_TRACE("rpc", "Remaining timeout: {}", remaining_timeout_.count());

    net::EventLoop loop;
    loop_ = &loop;
    net::TcpClient client(&loop, service_addr, RpcApplication::instance().serverName());
    client_ = &client;

    client.enableRetry();
    client.setConnectionCallback(std::bind(&RpcChannel::onConnection, this, std::placeholders::_1));
    client.setMessageCallback(std::bind(&RpcChannel::onMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3));

    // 启动响应定时器 防止超时 其中包含"连接服务提供者"和"接收服务提供者响应"两部分操作
    // 因此在接收到服务提供者的响应时再取消定时器
    response_timer_ = loop.runAfter(remaining_timeout_, std::bind(&RpcChannel::connectTimeout, this));

    client.connect();
    loop.loop();

    loop_   = nullptr;
    client_ = nullptr;

    if (is_timeout_) {
        controller->SetFailed("Response timeout");
    } else {
        // 解析响应数据
        if (!response->ParseFromString(buffer_)) {
            controller->SetFailed("Failed to parse response data form RpcProvider");
        }
        LOGGER_INFO("rpc", "Parse response data successfully");
    }

    if (done) done->Run();
}

void RpcChannel::createRequestContent(const std::string& service_name, const std::string& method_name, RpcControllerPtr controller, ConstMessagePtr request) {
    buffer_.clear();

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
    buffer_.append(std::string(reinterpret_cast<const char*>(&header_size), 4));
    buffer_.append(header_content);
    buffer_.append(args_content);

    LOGGER_TRACE("rpc", "Packaged RpcHeader: HeaderSize[{}] HeaderContent[{}] ServiceName[{}] MethodName[{}] ArgsSize[{}] Args[{}]",
        header_size, header_content, service_name, method_name, args_size, args_content);
}

void RpcChannel::onConnection(const net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        LOGGER_DEBUG("rpc", "Connect with RpcProvider[{}]", conn->peerAddress().toIpPort());
        conn->send(buffer_); // 发送请求内容
    } else {
        LOGGER_DEBUG("rpc", "Disconnect with RpcProvider[{}]", conn->peerAddress().toIpPort());
        conn->loop()->quit();
    }
}

void RpcChannel::onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time) {
    LOGGER_INFO("rpc", "Receive response data from RpcProvider[{}]", conn->peerAddress().toIpPort());
    conn->loop()->cancel(response_timer_); // 取消定时器
    buffer_.clear();
    buffer->readBytes(buffer_);
    assert(client_);
    client_->disconnect(); // 断开与服务提供者的连接
}

void RpcChannel::connectTimeout() {
    LOGGER_ERROR("rpc", "Response timeout");
    is_timeout_ = true;
    assert(client_ && loop_);
    client_->disconnect();
    loop_->quit();
}
} // namespace talko::rpc