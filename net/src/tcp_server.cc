#include <cassert>
#include <log/log.h>
#include <net/acceptor.h>
#include <net/byte_buffer.h>
#include <net/common.h>
#include <net/event_loop.h>
#include <net/tcp_connection.h>
#include <net/tcp_server.h>
#include <pool/thread_pool.h>

namespace talko::net {
/** 默认的连接回调函数 */
void defaultConnectionCallback(const TcpConnectionPtr& conn) {
    LOGGER_TRACE("net", "Connection between {} and {} is {}", conn->localAddress().toIpPort(),
        conn->peerAddress().toIpPort(), (conn->connected() ? "up" : "down"));
}

/** 默认的消息回调函数 */
void defaultMessageCallback(const TcpConnectionPtr&, ByteBuffer* buffer, TimePoint) {
    buffer->skipAllBytes();
}

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listen_addr, const std::string& name, bool resuse_port)
    : main_loop_(loop)
    , acceptor_(std::make_unique<Acceptor>(loop, listen_addr, resuse_port))
    , ip_port_(listen_addr.toIpPort())
    , name_(name)
    , connection_cb_(defaultConnectionCallback)
    , message_cb_(defaultMessageCallback) {
    assert(main_loop_ != nullptr && "EventLoop is nullptr");
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this,
        std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer() {
    main_loop_->checkIsInCreatorThread();
    LOGGER_TRACE("net", "Destory TcpServer[{}]", name_);

    // 销毁所有的连接
    for (auto& item : connections_) {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->loop()->runInLoop(std::bind(&TcpConnection::connectionDestoryed, conn));
    }
}

const std::string& TcpServer::ipPort() const {
    return ip_port_;
}

const std::string& TcpServer::name() const {
    return name_;
}

EventLoop* TcpServer::loop() const {
    return main_loop_;
}

void TcpServer::start() {
    if (!started_) {
        started_ = true;

        // 提交线程池任务
        assert(tp::isRunning() && "Thread pool is not started");
        for (size_t i = 0; i < sub_loop_size_; ++i) {
            tp::submitTask(std::bind(&TcpServer::startSubEventLoop, this));
        }

        assert(!acceptor_->listening());
        main_loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

void TcpServer::setSubLoopSize(size_t size) {
    assert(!started_);
    if (tp::mode() == tp::ThreadPoolMode::fixed) {
        assert(tp::idleThreadSize() >= size && "Size of subloop is less than thread num of thread pool");
    }
    sub_loop_size_ = size;
}

void TcpServer::setConnectionCallback(ConnectionCallback cb) {
    connection_cb_ = std::move(cb);
}

void TcpServer::setMessageCallback(MessageCallback cb) {
    message_cb_ = std::move(cb);
}

void TcpServer::setWriteCompleteCallback(WriteCompleteCallback cb) {
    write_complete_cb_ = std::move(cb);
}

void TcpServer::newConnection(int sockfd, const InetAddress& peer_addr) {
    main_loop_->checkIsInCreatorThread();

    // 获取下一个事件循环
    EventLoop* cur_loop = getNextLoop();

    // 生成连接名称
    std::string conn_name = fmt::format("{}-{}#{}", name_, ip_port_, nxt_conn_id_);
    ++nxt_conn_id_;

    LOGGER_INFO("net", "New connection {} with {} on socket fd {}", conn_name, peer_addr.toIpPort(), sockfd);
    InetAddress local_addr(common::getLocalAddr(sockfd));

    // 创建新连接
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(cur_loop, conn_name, sockfd, local_addr, peer_addr);

    // 指定连接相应的回调函数
    connections_[conn_name] = conn;
    conn->setConnectionCallback(connection_cb_);
    conn->setMessageCallback(message_cb_);
    conn->setWriteCompleteCallback(write_complete_cb_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    // 连接建立成功 监听连接上的可读事件
    cur_loop->runInLoop(std::bind(&TcpConnection::connectionEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
    main_loop_->runInLoop(std::bind(&TcpServer::removeConnection_, this, conn));
}

void TcpServer::removeConnection_(const TcpConnectionPtr& conn) {
    main_loop_->checkIsInCreatorThread();
    LOGGER_INFO("net", "Remove connection {} from {}", conn->name(), name_);
    connections_.erase(conn->name());
    conn->loop()->runInLoop(std::bind(&TcpConnection::connectionDestoryed, conn));
}

void TcpServer::startSubEventLoop() {
    EventLoop sub_loop;

    {
        std::lock_guard<std::mutex> lock(mtx_);
        sub_loops_.push_back(&sub_loop);
    }

    // 开启循环
    sub_loop.loop();

    // 销毁时不修改容器内容 仅仅将对应索引的指针赋为空 无需枷锁
    auto iter = std::find(sub_loops_.begin(), sub_loops_.end(), &sub_loop);
    *iter     = nullptr;
}

EventLoop* TcpServer::getNextLoop() {
    assert(started_);
    EventLoop* cur_loop = main_loop_;

    if (!sub_loops_.empty()) {
        cur_loop = sub_loops_[next_];
        ++next_;
        next_ %= sub_loop_size_;
    }
    return cur_loop;
}
} // namespace talko::net