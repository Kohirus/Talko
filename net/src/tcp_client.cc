#include <cassert>
#include <log/log.h>
#include <net/connector.h>
#include <net/event_loop.h>
#include <net/tcp_client.h>
#include <net/tcp_connection.h>

namespace talko::net {
TcpClient::TcpClient(EventLoop* loop, const InetAddress& server_addr, const std::string& name)
    : loop_(loop)
    , connector_(std::make_shared<Connector>(loop, server_addr))
    , name_(name)
    , connection_cb_(defaultConnectionCallback)
    , message_cb_(defaultMessageCallback) {
    assert(loop != nullptr);
    LOGGER_DEBUG("net", "Create TcpClinet[{}] at {}", name_, fmt::ptr(this));
    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection,
        this, std::placeholders::_1));
}

TcpClient::~TcpClient() {
    LOGGER_DEBUG("net", "Desotry TcpClient[{}] at {}", name_, fmt::ptr(this));
    TcpConnectionPtr conn;

    bool unique = false;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        unique = connection_.unique();
        conn   = connection_;
    }
    if (conn) {
        assert(loop_ == conn->loop());
        CloseCallback cb = std::bind([&]() {
            loop_->queueInLoop(std::bind(&TcpConnection::connectionDestoryed, conn));
        });
        loop_->runInLoop(std::bind(&TcpConnection::setCloseCallback, conn, cb));
        if (unique) {
            conn->forceClose();
        }
    } else {
        connector_->stop();
        loop_->runAfter(std::chrono::seconds(1), [&]() {
            connector_.reset();
        });
    }
}

void TcpClient::connect() {
    LOGGER_INFO("net", "TcpClient[{}] connecting to {}", name_,
        connector_->serverAddress().toIpPort());
    connect_ = true;
    connector_->start();
}

void TcpClient::disconnect() {
    connect_ = false;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (connection_) {
            connection_->shutdown();
        }
    }
}

void TcpClient::stop() {
    connect_ = false;
    connector_->stop();
}

TcpConnectionPtr TcpClient::connection() const {
    return connection_;
}

EventLoop* TcpClient::loop() const {
    return loop_;
}

bool TcpClient::retry() const {
    return retry_;
}

void TcpClient::enableRetry() {
    retry_ = true;
}

const std::string& TcpClient::name() const {
    return name_;
}

void TcpClient::setConnectionCallback(ConnectionCallback cb) {
    connection_cb_ = std::move(cb);
}

void TcpClient::setMessageCallback(MessageCallback cb) {
    message_cb_ = std::move(cb);
}

void TcpClient::setWriteCompleteCallback(WriteCompleteCallback cb) {
    write_complete_cb_ = std::move(cb);
}

void TcpClient::newConnection(int sockfd) {
    // Connector成功与服务器建立连接后会销毁其Channel资源
    // 并将连接套接字sockfd传递至此 交给TcpConnection进行管理
    loop_->checkIsInCreatorThread();

    // 生成连接名称
    InetAddress peer_addr(common::getPeerAddr(sockfd));
    std::string conn_name = fmt::format("{}:{}#{}", name_, peer_addr.toIpPort(), nxt_conn_id_++);

    // 构建连接管理器
    InetAddress      local_addr(common::getLocalAddr(sockfd));
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(loop_, conn_name, sockfd, local_addr, peer_addr);

    // 设置相应的回调函数
    conn->setConnectionCallback(connection_cb_);
    conn->setMessageCallback(message_cb_);
    conn->setWriteCompleteCallback(write_complete_cb_);
    conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));

    {
        std::lock_guard<std::mutex> lock(mtx_);
        connection_ = conn;
    }

    conn->connectionEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn) {
    // 由于当发生套接字挂起事件, 即远端关闭连接或主动关闭套接字时
    // 套接字所对应的Channel会调用相应的CloseCallback回调函数
    // 即此处的removeConnection
    // 那么如果是远端关闭连接 说明服务器出现了问题 如果客户端开启了重连选项
    // 则需要进行重连尝试
    // 如果是主动关闭套接字 则此时connect_会被置为false 则无需重连尝试
    loop_->checkIsInCreatorThread();
    assert(loop_ == conn->loop());

    // 此处需要先清理连接器的资源 因为在尝试重连成功后
    // 会调用相应的连接成功回调函数 即TcpClient::newConnection
    // 又会重新建立新的连接器
    {
        std::lock_guard<std::mutex> lock(mtx_);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->queueInLoop(std::bind(&TcpConnection::connectionDestoryed, conn));
    if (retry_ && connect_) {
        LOGGER_INFO("net", "Reconnecting to {}", connector_->serverAddress().toIpPort());
        connector_->restart();
    }
}

} // namespace talko::net