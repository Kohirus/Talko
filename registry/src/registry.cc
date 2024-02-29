#include <registry/register.pb.h>
#include <registry/registry.h>
#include <rpc/rpc_application.h>

namespace talko::registry {
Registry::Registry(net::EventLoop* loop)
    : server_(loop, rpc::RpcApplication::instance().serverAddress(),
        rpc::RpcApplication::instance().serverName(),
        rpc::RpcApplication::instance().reusePort())
    , manager_(std::make_unique<ServiceManager>()) {
    server_.setSubLoopSize(rpc::RpcApplication::instance().subloopSize());
    server_.setConnectionCallback(std::bind(&Registry::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(&Registry::onMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3));
}

void Registry::start() {
    server_.start();
}

void Registry::onConnection(const net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        log::info("New connection: {}", conn->peerAddress().toIpPort());
    } else {
        log::info("Connection with {} destoryed", conn->peerAddress().toIpPort());
    }
}

void Registry::onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time) {
    std::string message;
    buffer->readBytes(message);

    ServiceRequest request;

    // 解析请求数据
    if (!request.ParseFromString(message)) {
        log::error("Failed to parse request from {}", conn->peerAddress().toIpPort());
        requestError(conn, "Failed to parse request");
        return;
    }

    RequestType type = request.request_type();

    // 查看是否具有请求实例对象
    if (!request.has_instance()) {
        log::error("Instance of request is null from {}", conn->peerAddress().toIpPort());
        requestError(conn, "Instance of request is null");
        return;
    }

    // 获取实例对象的相关信息
    std::string service_name = request.instance().service_name();
    std::string method_name  = request.instance().method_name();
    if (service_name.empty() || method_name.empty()) {
        log::error("Name is empty from {}: [{}]-[{}]", conn->peerAddress().toIpPort(),
            service_name, method_name);
        requestError(conn, "ServiceName or MethodName is empty");
        return;
    }

    if (type == RequestType::REGISTER) { // 注册
        registerMethod(service_name, method_name, conn);
    } else if (type == RequestType::DISCOVER) { // 发现
        discoverMethod(service_name, method_name, conn);
    } else {
        assert(0);
        log::error("Unexpected RequestType: {}", static_cast<int>(type));
    }
}

void Registry::registerMethod(const std::string& service_name, const std::string& method_name, const net::TcpConnectionPtr& conn) {
    log::info("Register new service from {}: [{}]-[{}]", conn->peerAddress().toIpPort(),
        service_name, method_name);

    // 添加服务
    if (!manager_->addService(service_name, conn->peerAddress().toIp(), conn->peerAddress().port())) {
        log::error("Failed to add new service: {}", service_name);
        requestError(conn, fmt::format("Failed to add new service: {}", service_name));
    }

    // 添加方法
    if (!manager_->addMethod(service_name, method_name)) {
        log::error("Failed to add new method: {}", method_name);
        requestError(conn, fmt::format("Failed to add new method: {}", method_name));
    }

    // 组织实例内容
    ServiceInstance instance;
    instance.set_service_name(service_name);
    instance.set_method_name(method_name);
    instance.set_address(conn->peerAddress().toIp());
    instance.set_port(conn->peerAddress().port());
    requestSuccess(conn, &instance);
}

void Registry::discoverMethod(const std::string& service_name, const std::string& method_name, const net::TcpConnectionPtr& conn) {
    log::info("Discover existed service from {}: [{}]-[{}]", conn->peerAddress().toIpPort(),
        service_name, method_name);

    auto info = manager_->find(service_name, method_name);
    if (!info.has_value()) {
        log::error("Failed to find Service[{}] and Method[{}]", service_name, method_name);
        requestError(conn, fmt::format("Failed to find Service[{}] and Method[{}]", service_name, method_name));
    }

    auto [ip, port] = info.value();

    // 组织实例内容
    ServiceInstance instance;
    instance.set_service_name(service_name);
    instance.set_method_name(method_name);
    instance.set_address(ip);
    instance.set_port(port);
    requestSuccess(conn, &instance);
}

void Registry::requestSuccess(const net::TcpConnectionPtr& conn, ServiceInstance* instance) {
    // 组织响应数据
    ServiceResponse response;
    response.set_success(true);
    response.set_allocated_instance(instance);

    // 向对端发送响应数据
    std::string result = response.SerializeAsString();
    conn->send(result);
}

void Registry::requestError(const net::TcpConnectionPtr& conn, const std::string& err_msg) {
    ServiceResponse response;
    response.set_success(false);
    response.set_err_msg(err_msg);
    std::string result = response.SerializeAsString();
    conn->send(result);
}
} // namespace talko::registry