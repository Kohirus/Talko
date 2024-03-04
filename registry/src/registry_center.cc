#include <registry/registry_center.h>
#include <rpc/rpc_application.h>

namespace talko::registry {
RegistryCenter::RegistryCenter(net::EventLoop* loop)
    : server_(loop, rpc::RpcApplication::instance().serverAddress(),
        rpc::RpcApplication::instance().serverName(),
        rpc::RpcApplication::instance().reusePort())
    , manager_(std::make_unique<ServiceManager>())
    , heartbeat_timeout_(std::chrono::seconds(11)) {
    server_.setSubLoopSize(rpc::RpcApplication::instance().subloopSize());
    server_.setConnectionCallback(std::bind(&RegistryCenter::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(&RegistryCenter::onMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3));
    // 开启心跳检测定时器
    server_.loop()->runEvery(heartbeat_timeout_, std::bind(&RegistryCenter::handleHeartbeatTimeout, this));
}

void RegistryCenter::start() {
    LOG_INFO("Start RegistryCenter");
    server_.start();
}

void RegistryCenter::onConnection(const net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        LOG_INFO("New connection: {}", conn->peerAddress().toIpPort());
        // 将该连接添加到连接列表中 存储service_name是为了后序该服务未发送心跳包
        // 则将其从service_manager中删除 同时通知所有的服务请求者
        // 这里由于不知道是服务提供方还是服务请求方 将其service_name设为空即可
        conns_.insert({ conn, std::make_pair("", true) });
    } else {
        if (!conns_[conn].first.empty()) {
            LOG_INFO("Connection with {} destoryed, remove Service[{}]", conn->peerAddress().toIpPort(),
                conns_[conn].first);
            manager_->removeService(conns_[conn].first); // 从服务映射表中删除当前连接的服务
            broadcast(conns_[conn].first);               // 广播服务下线的消息
        } else {
            LOG_INFO("Connection with {} destoryed", conn->peerAddress().toIpPort());
        }
        conns_.erase(conn); // 将当前连接从连接表中移除
    }
}

void RegistryCenter::onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time) {
    std::string message;
    buffer->readBytes(message);

    ServiceRequest request;

    // 解析请求数据
    if (!request.ParseFromString(message)) {
        LOG_ERROR("Failed to parse request from {}", conn->peerAddress().toIpPort());
        requestError(MessageType::REGISTER, conn, "Failed to parse request");
        return;
    }

    MessageType type = request.msg_type();
    if (type == MessageType::HEARTBEAT) {
        connectionAlive(conn);
        return;
    }

    // 查看是否具有请求实例对象
    if (!request.has_instance()) {
        LOG_ERROR("Instance of request is null from {}", conn->peerAddress().toIpPort());
        requestError(type, conn, "Instance of request is null");
        return;
    }

    // 获取实例对象的相关信息
    std::string service_name = request.instance().service_name();
    std::string method_name  = request.instance().method_name();
    if (service_name.empty() || method_name.empty()) {
        LOG_ERROR("Name is empty from {}: [{}]-[{}]", conn->peerAddress().toIpPort(),
            service_name, method_name);
        requestError(type, conn, "ServiceName or MethodName is empty");
        return;
    }

    if (type == MessageType::REGISTER) { // 注册
        std::string      ip   = request.instance().address();
        uint16_t         port = request.instance().port();
        net::InetAddress provider_addr(ip, port);
        enrollMethod(service_name, method_name, provider_addr, conn);
    } else if (type == MessageType::DISCOVER) { // 发现
        discoverMethod(service_name, method_name, conn);
    } else {
        assert(0);
        LOG_ERROR("Unexpected RequestType: {}", static_cast<int>(type));
    }
}

void RegistryCenter::enrollMethod(const std::string& service_name, const std::string& method_name, const net::InetAddress& proriver_addr, const net::TcpConnectionPtr& conn) {
    LOG_INFO("Enroll new service from {}: [{}]-[{}]-[{}]", conn->peerAddress().toIpPort(),
        service_name, method_name, proriver_addr.toIpPort());

    if (!manager_->serviceExist(service_name)) {
        // 如果服务不存在则添加服务
        if (!manager_->addService(service_name, proriver_addr.toIp(), proriver_addr.port())) {
            LOG_ERROR("Failed to add new service: {}", service_name);
            requestError(MessageType::REGISTER, conn, fmt::format("Failed to add new service: {}", service_name));
            return;
        }
    }

    // 添加方法
    if (!manager_->addMethod(service_name, method_name)) {
        LOG_ERROR("Failed to add new method: {}", method_name);
        requestError(MessageType::REGISTER, conn, fmt::format("Failed to add new method: {}", method_name));
        return;
    }

    // 组织实例内容
    ServiceInstance* instance = new ServiceInstance;
    instance->set_service_name(service_name);
    instance->set_method_name(method_name);
    instance->set_address(proriver_addr.toIp());
    instance->set_port(proriver_addr.port());
    LOG_INFO("{} enroll successfully", conn->peerAddress().toIpPort());
    requestSuccess(MessageType::REGISTER, conn, instance);

    // 为该连接添加服务名称方法
    conns_[conn] = std::make_pair(service_name, true);
}

void RegistryCenter::discoverMethod(const std::string& service_name, const std::string& method_name, const net::TcpConnectionPtr& conn) {
    LOG_INFO("Request for discovering [{}]-[{}] from {}", service_name, method_name, conn->peerAddress().toIpPort());

    auto info = manager_->find(service_name, method_name);
    if (!info.has_value()) {
        LOG_ERROR("Failed to find Service[{}] and Method[{}]", service_name, method_name);
        requestError(MessageType::DISCOVER, conn, fmt::format("Failed to find Service[{}] and Method[{}]", service_name, method_name));
        return;
    }

    auto [ip, port] = info.value();

    // 组织实例内容
    ServiceInstance* instance = new ServiceInstance;
    instance->set_service_name(service_name);
    instance->set_method_name(method_name);
    instance->set_address(ip);
    instance->set_port(port);
    requestSuccess(MessageType::DISCOVER, conn, instance);
}

void RegistryCenter::requestSuccess(MessageType type, const net::TcpConnectionPtr& conn, ServiceInstance* instance) {
    // 组织响应数据
    ServiceResponse response;
    response.set_msg_type(type);
    response.set_success(true);
    response.set_allocated_instance(instance);

    // 向对端发送响应数据
    std::string result;
    if (!response.SerializeToString(&result)) {
        LOG_FATAL("Failed to serialize response data");
    }
    LOG_DEBUG("Send success response to the {}", conn->peerAddress().toIpPort());

    std::string tmp;
    for (auto ch : result) {
        tmp += fmt::format("{:02x}", ch);
    }
    LOG_DEBUG("Response data: {}", tmp);

    conn->send(result);
}

void RegistryCenter::requestError(MessageType type, const net::TcpConnectionPtr& conn, const std::string& err_msg) {
    // 组织响应数据
    ServiceResponse response;
    response.set_msg_type(type);
    response.set_success(false);
    response.set_err_msg(err_msg);

    // 向对端发送响应数据
    std::string result;
    if (!response.SerializeToString(&result)) {
        LOG_FATAL("Failed to serialize response data");
    }
    LOG_DEBUG("Send error response to the {}", conn->peerAddress().toIpPort());
    conn->send(result);
}

void RegistryCenter::connectionAlive(const net::TcpConnectionPtr& conn) {
    LOG_DEBUG("Heartbeat data form {}", conn->peerAddress().toIpPort());
    conns_[conn].second = true;
}

void RegistryCenter::handleHeartbeatTimeout() {
    if (conns_.empty()) return;
    size_t death_conn_cnt = 0; // 死亡连接的数目

    // 遍历服务提供者映射表 找出其中已死亡的连接
    for (auto iter = conns_.begin(); iter != conns_.end(); ++iter) {
        auto& [service_name, alive] = iter->second;
        if (!alive) {
            if (!service_name.empty()) {
                // 服务提供方需要从服务管理表中删除相应的服务名称
                LOG_DEBUG("Service[{}] from {} is death, remove it", service_name,
                    iter->first->peerAddress().toIpPort());
                manager_->removeService(service_name); // 让服务管理器删除该服务
            } else {
                LOG_DEBUG("Requester from {} is death, remove it", iter->first->peerAddress().toIpPort());
            }
            ++death_conn_cnt;
            iter->first->forceClose(); // 关闭已死亡的连接 触发连接回调 广播服务已下线 同时删除死亡连接
        } else {
            // 如果该连接存活 则重置其死亡标志
            alive = false;
        }
    }

    if (death_conn_cnt == 0) {
        LOG_DEBUG("No connection dead");
    } else {
        LOG_DEBUG("The number of dead connection: {}", death_conn_cnt);
    }
}

void RegistryCenter::broadcast(const std::string& service_name) {
    // 组织广播数据
    ServiceInstance* instance = new ServiceInstance;
    instance->set_service_name(service_name);

    ServiceResponse response;
    response.set_msg_type(MessageType::BROADCAST);
    response.set_success(true);
    response.set_allocated_instance(instance);

    std::string response_content;
    if (!response.SerializeToString(&response_content)) {
        LOG_FATAL("Failed to serialize broadcast data");
    }

    for (auto iter = conns_.begin(); iter != conns_.end(); ++iter) {
        auto& [name, alive] = iter->second;
        if (name != service_name) {
            LOG_DEBUG("Notify {} service named {} is dead", iter->first->peerAddress().toIpPort(), service_name);
            iter->first->send(response_content);
        }
    }

    LOG_DEBUG("Boradcast end");
}
} // namespace talko::registry