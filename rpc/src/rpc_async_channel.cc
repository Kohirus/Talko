#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <rpc/rpc_application.h>
#include <rpc/rpc_async_channel.h>
#include <rpc/rpc_header.pb.h>

namespace talko::rpc {
RpcAsyncChannel::RpcAsyncChannel(net::Duration timeout)
    : timeout_(timeout) { }

void RpcAsyncChannel::CallMethod(MethodDescriptorPtr method, RpcControllerPtr controller,
    ConstMessagePtr request, MessagePtr response, ClosurePtr done) {
    assert(tp::isRunning() && "Thread pool is stop");

    ServiceDescriptorPtr service = method->service();
    // 获取服务名称和方法名称
    std::string service_name = service->name();
    std::string method_name  = method->name();

    tp::submitTask(std::bind(&RpcAsyncChannel::handleRequest, this, service_name,
        method_name, controller, request, response, done));
}

void RpcAsyncChannel::onConnection(const net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        LOGGER_INFO("rpc", "Connection established with RpcProvider");
        conn->send(package_);
        LOGGER_TRACE("rpc", "Send package to RpcProvider: {}", package_);
    } else {
        LOGGER_INFO("rpc", "Connection destoryed with RpcProvider");
        conn->loop()->quit();
    }
}

void RpcAsyncChannel::onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time) {
    buffer->readBytes(result_);
    LOGGER_TRACE("rpc", "Receive response: {}", result_);
    assert(client_);
    client_->disconnect();
}

void RpcAsyncChannel::handleTimeout(RpcControllerPtr controller) {
    controller->SetFailed("RPC request is timeout");
    controller->StartCancel();
    assert(client_);
    client_->stop();
    client_->loop()->quit();
}

void RpcAsyncChannel::handleRequest(const std::string& service_name, const std::string& method_name,
    RpcControllerPtr controller, ConstMessagePtr request, MessagePtr response, ClosurePtr done) {
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

    net::EventLoop loop;

    // 建立TCP连接
    client_ = std::make_unique<net::TcpClient>(&loop, rpc::RpcApplication::instance().serverAddress(), "RpcRequester");
    client_->enableRetry();
    client_->setConnectionCallback(std::bind(&RpcAsyncChannel::onConnection, this, std::placeholders::_1));
    client_->setMessageCallback(std::bind(&RpcAsyncChannel::onMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3));
    client_->connect();

    loop.runAfter(timeout_, std::bind(&RpcAsyncChannel::handleTimeout, this, controller));
    loop.loop();

    if (!controller->Failed()) {
        LOGGER_DEBUG("rpc", "RPC Request is complete");
        // 对接收到的RPC应答进行反序列化
        if (!response->ParseFromString(result_)) {
            controller->SetFailed("Failed to parse response data");
        } else {
            LOGGER_TRACE("rpc", "Response parsing is complete");
        }
    }

    client_.reset();
    if (done) done->Run();
}
} // namespace talko::rpc