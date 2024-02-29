#include <rpc/rpc_application.h>
#include <rpc/rpc_registry.h>
#include <thread>

namespace talko::rpc {
RpcRegistry::RpcRegistry(net::Duration timeout)
    : timeout_(timeout) {
}

RpcRegistry::~RpcRegistry() {
}

bool RpcRegistry::connect() {
    assert(tp::isRunning() && "Thread Pool is stop");
    tp::submitTask(std::bind(&RpcRegistry::connect_, this));

    std::unique_lock<std::mutex> lock(mtx_);
    // 等待事件循环创建成功
    cond_.wait(lock, [&]() -> bool { return loop_ != nullptr; });

    while (!connected_ && loop_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return connected_;
}

void RpcRegistry::connect_() {
    net::EventLoop loop;

    {
        std::lock_guard<std::mutex> lock(mtx_);
        loop_ = &loop;
    }
    cond_.notify_one();

    net::TcpClient client(&loop, RpcApplication::instance().registerAddress(),
        RpcApplication::instance().serverName());
    client.enableRetry();
    client.setConnectionCallback(std::bind(&RpcRegistry::onConnection, this, std::placeholders::_1));
    client.setMessageCallback(std::bind(&RpcRegistry::onMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3));

    // 开启定时器 防止连接超时
    timer_id_ = loop_->runAfter(timeout_, std::bind(&RpcRegistry::connectTimeout, this));

    client.connect();
    loop.loop();

    loop_ = nullptr;
}

void RpcRegistry::onConnection(const net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        LOGGER_INFO("rpc", "Connect with Register");
        connected_ = true;
        loop_->cancel(timer_id_);
    } else {
        loop_->quit();
    }
}

void RpcRegistry::onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time) {
}

void RpcRegistry::connectTimeout() {
    err_msg_ = "Register connect timeout";
    loop_->quit();
}
} // namespace talko::rpc