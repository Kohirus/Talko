#include <rpc/rpc_application.h>
#include <rpc/rpc_regedit.pb.h>

namespace talko::rpc {
RpcRegistrant& RpcRegistrant::instance() {
    static RpcRegistrant reg;
    return reg;
}

RpcRegistrant::RpcRegistrant()
    : connect_timeout_(std::chrono::seconds(10))
    , heartbeat_interval_(std::chrono::seconds(10)) {
}

RpcRegistrant::~RpcRegistrant() {
    assert(loop_);
    loop_->cancel(heartbeat_timer_); // 关闭心跳包
    client_->disconnect();
    LOGGER_TRACE("rpc", "Wait for the task to the end");
    task_ret_.wait();
}

bool RpcRegistrant::connect(net::Duration connect_timeout, net::Duration heartbeat_interval, const net::InetAddress& server_addr) {
    connect_timeout_    = connect_timeout;
    heartbeat_interval_ = heartbeat_interval;
    registry_addr_      = server_addr;

    assert(pool::isThreadPoolRunning() && "Thread Pool is stop");

    // 防止多次启动
    if (connected_) return true;

    task_ret_ = pool::submitTask(std::bind(&RpcRegistrant::connect_, this));

    std::unique_lock<std::mutex> lock(mtx_);

    // 等待事件循环创建成功
    cond_.wait(lock, [&]() -> bool { return loop_ != nullptr; });

    // 阻塞当前线程以检查是否连接到注册器 如果超时则loop_会变为nullptr
    cond_.wait(lock, [&]() -> bool { return connected_ || !loop_; });

    return connected_;
}

bool RpcRegistrant::enrollMethod(const std::string& service_name, const std::string& method_name, net::Duration timeout) {
    assert(loop_ != nullptr && "Not connected to Registry");

    // 重置相关变量
    response_finished_ = false;
    response_success_  = false;
    resetErrorMessage();

    // 让子线程执行注册任务
    loop_->queueInLoop(std::bind(&RpcRegistrant::enrollMethod_, this, service_name, method_name));

    {
        // 等待注册超时或接收到注册响应
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait_for(lock, timeout, [&]() -> bool { return response_finished_; });
    }

    if (!response_finished_) {
        // 如果响应超时则等待loop_为空 即等待子线程退出再返回
        setErrorMessage("Request timeout");

        // 断开连接
        client_->disconnect();

        // 等待子线程退出
        LOGGER_TRACE("rpc", "Wait for the task to the end");
        task_ret_.wait();
    }

    return response_success_;
}

bool RpcRegistrant::discoverMethod(const std::string& service_name, const std::string& method_name, net::Duration timeout, net::InetAddress& provider_addr) {
    // 在本地缓存查询是否存在该服务
    if (isServiceExistInCache(service_name, method_name, provider_addr)) {
        LOGGER_INFO("rpc", "Find [{}]-[{}] in the cache, it is located on {}", service_name,
            method_name, provider_addr.toIpPort());
        return true;
    }

    assert(loop_ != nullptr && "Not connected to Registry");
    LOGGER_DEBUG("rpc", "Not find [{}]-[{}] in the cache, so discover in the RegistryCenter", service_name, method_name);

    // 重置相关变量
    response_finished_ = false;
    response_success_  = false;
    resetErrorMessage();

    // 让子线程执行注册任务
    loop_->queueInLoop(std::bind(&RpcRegistrant::discoverMethod_, this, service_name, method_name));

    {
        // 等待发现超时或接收到发现响应
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait_for(lock, timeout, [&]() -> bool { return response_finished_; });
    }

    if (!response_finished_) {
        // 如果发现超时则等待loop_为空 即等待子线程退出再返回
        setErrorMessage("Request timeout");

        // 断开连接
        client_->disconnect();

        // 等待子线程退出
        LOGGER_TRACE("rpc", "Wait for the task to the end");
        task_ret_.wait();
    }

    if (response_success_) {
        // 响应成功则将其存入缓存
        provider_addr = service_addr_;
        saveServiceInCache(service_name, method_name, provider_addr);
        LOGGER_DEBUG("rpc", "Save [{}]-[{}]-[{}] in the cache", service_name, method_name, provider_addr.toIpPort());
    }

    return response_success_;
}

std::string RpcRegistrant::errorMessage() {
    std::lock_guard<std::mutex> lock(mtx_);
    return err_msg_;
}

void RpcRegistrant::resetErrorMessage() {
    std::lock_guard<std::mutex> lock(mtx_);
    err_msg_.clear();
}

void RpcRegistrant::setErrorMessage(const std::string& err_msg) {
    std::lock_guard<std::mutex> lock(mtx_);
    err_msg_ = err_msg;
}

void RpcRegistrant::connect_() {
    net::EventLoop loop;

    loop_ = &loop;
    cond_.notify_one();

    net::TcpClient client(&loop, registry_addr_, RpcApplication::instance().serverName());
    client_ = &client;
    client.enableRetry();
    client.setConnectionCallback(std::bind(&RpcRegistrant::onConnection, this, std::placeholders::_1));
    client.setMessageCallback(std::bind(&RpcRegistrant::onMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3));

    // 开启定时器 防止连接超时
    connect_timer_ = loop_->runAfter(connect_timeout_, std::bind(&RpcRegistrant::connectTimeout, this));

    client.connect();
    loop.loop();

    conn_.reset();
    loop_   = nullptr;
    client_ = nullptr;
    cond_.notify_one();
}

void RpcRegistrant::onConnection(const net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        LOGGER_INFO("rpc", "Connect with RegistryCenter");
        connected_ = true;
        cond_.notify_one();            // 通知主线程连接成功
        loop_->cancel(connect_timer_); // 关闭连接超时定时器

        // 开启心跳包 使注册中心确认当前RPC节点的存活
        heartbeat_timer_ = loop_->runEvery(heartbeat_interval_, std::bind(&RpcRegistrant::heartbeat, this, conn));

        // 记录与注册中心的连接
        conn_ = conn;
    } else {
        LOGGER_INFO("rpc", "Disconnect with RegistryCenter");
        conn_.reset();
        loop_->quit();
    }
}

void RpcRegistrant::onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time) {
    LOGGER_TRACE("rpc", "Receive response data");

    std::string message;
    buffer->readBytes(message);

    // 反序列化响应数据
    registry::ServiceResponse response;
    if (!response.ParseFromString(message)) {
        LOGGER_ERROR("rpc", "Failed to parse response form RegistryCenter");
        setErrorMessage("Failed to parse response from RegistryCenter");
        response_finished_ = true;
        response_success_  = false;
        cond_.notify_one(); // 通知主线程响应完成
        return;
    }

    registry::MessageType type = response.msg_type();

    if (!response.success()) { // 响应失败
        LOGGER_ERROR("rpc", "Response has fault");
        setErrorMessage(response.err_msg());
        response_success_ = false;
    } else { // 响应成功
        LOGGER_TRACE("rpc", "Response is success");
        switch (type) {
        case registry::MessageType::DISCOVER: {
            // 如果响应类型为发现服务则需要写入RPC服务地址 同时将其存入缓存
            assert(response.has_instance());
            std::string ip   = response.instance().address();
            uint16_t    port = response.instance().port();
            LOGGER_DEBUG("rpc", "Response type is DISCOVER, ip is {}, port is {}", ip, port);
            service_addr_ = net::InetAddress(ip, port);
        } break;
        case registry::MessageType::BROADCAST: {
            // 如果响应类型为广播服务 则查询当前缓存中是否存在该服务 存在则删除
            assert(response.has_instance());
            std::string service_name = response.instance().service_name();
            LOGGER_DEBUG("rpc", "Response type is BROADCAST, service named {} is death", service_name);
            removeServiceInCache(service_name);
        } break;
        default:
            LOGGER_TRACE("rpc", "Receive other type response");
            break;
        }

        resetErrorMessage();
        response_success_ = true;
    }

    response_finished_ = true;
    cond_.notify_one();
}

void RpcRegistrant::connectTimeout() {
    setErrorMessage("Enroll connected timeout");
    client_->disconnect();
    loop_->quit();
}

void RpcRegistrant::heartbeat(const net::TcpConnectionPtr& conn) {
    // 定时向注册中心发送心跳包，由注册中心确定RPC节点的存活状态
    // 如果在固定时间间隔内并没有受到来自RPC节点的心跳数据
    // 则注册中心认为该RPC节点已经死亡则通知其它订阅该死亡节点的RPC节点
    // 此处并未进行更为复杂的处理，比如心跳超时后，可能只是因为网络环境不好
    // 而导致心态包并没有及时收到，此时，注册中心节点可以向该RPC节点发送
    // 确认存活的数据包或者重新连接该RPC节点等。

    registry::ServiceRequest request;
    request.set_msg_type(registry::MessageType::HEARTBEAT);
    request.set_allocated_instance(nullptr);

    LOGGER_TRACE("rpc", "Send heartbeat data to RegistryCenter");

    std::string res;
    if (!request.SerializeToString(&res)) {
        LOGGER_FATAL("rpc", "Failed to serialize heartbeat data");
    }
    conn->send(res);
}

void RpcRegistrant::enrollMethod_(const std::string& service_name, const std::string& method_name) {
    LOGGER_TRACE("rpc", "Try to enroll [{}]-[{}] to RegistryCenter", service_name, method_name);

    // 设置注册实例内容
    registry::ServiceInstance* instance = new registry::ServiceInstance;
    instance->set_service_name(service_name);
    instance->set_method_name(method_name);
    instance->set_address(RpcApplication::instance().ip());
    instance->set_port(RpcApplication::instance().port());

    // 设置请求内容
    registry::ServiceRequest request;
    request.set_msg_type(registry::MessageType::REGISTER);
    request.set_allocated_instance(instance);

    // 序列化请求数据并向对端发送
    std::string result;
    if (!request.SerializeToString(&result)) {
        LOGGER_FATAL("rpc", "Failed to serialize enroll request");
    }
    LOGGER_TRACE("rpc", "Send enroll request to the RegistryCenter");
    conn_->send(result);
}

void RpcRegistrant::discoverMethod_(const std::string& service_name, const std::string& method_name) {
    LOGGER_TRACE("rpc", "Try to discover [{}]-[{}] from RegistryCenter", service_name, method_name);

    // 设置注册实例内容
    registry::ServiceInstance* instance = new registry::ServiceInstance;
    instance->set_service_name(service_name);
    instance->set_method_name(method_name);

    // 设置请求内容
    registry::ServiceRequest request;
    request.set_msg_type(registry::MessageType::DISCOVER);
    request.set_allocated_instance(instance);

    // 序列化请求数据并向对端发送
    std::string result;
    if (!request.SerializeToString(&result)) {
        LOGGER_FATAL("rpc", "Failed to serialize discover request");
    }
    LOGGER_TRACE("rpc", "Send discover request to the RegistryCenter");
    conn_->send(result);
}

bool RpcRegistrant::isServiceExistInCache(const std::string& service_name, const std::string& method_name, net::InetAddress& provider_addr) {
    std::shared_lock<std::shared_mutex> lock(services_mtx_);

    auto iter_srv = services_.find(service_name);
    if (iter_srv == services_.end()) {
        return false;
    }

    auto iter_mtd = iter_srv->second.methods.find(method_name);
    if (iter_mtd != iter_srv->second.methods.end()) {
        provider_addr = iter_srv->second.addr;
        return true;
    }

    return false;
}

void RpcRegistrant::saveServiceInCache(const std::string& service_name, const std::string& method_name, const net::InetAddress& provider_addr) {
    ServiceInfo info;
    info.addr = provider_addr;
    info.methods.insert(method_name);

    std::unique_lock<std::shared_mutex> lock(services_mtx_);
    services_.insert({ service_name, info });
}

void RpcRegistrant::removeServiceInCache(const std::string& service_name) {
    std::unique_lock<std::shared_mutex> lock(services_mtx_);

    auto iter = services_.find(service_name);
    if (iter != services_.end()) {
        LOGGER_DEBUG("rpc", "Service named {} is found in the cache, remove it", service_name);
        services_.erase(iter);
    }
}
} // namespace talko::rpc