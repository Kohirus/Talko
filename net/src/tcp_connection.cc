#include <log/log.h>
#include <net/channel.h>
#include <net/common.h>
#include <net/event_loop.h>
#include <net/socket.h>
#include <net/tcp_connection.h>

namespace talko::net {
TcpConnection::TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
    const InetAddress& local_addr, const InetAddress& peer_addr)
    : loop_(loop)
    , name_(name)
    , socket_(std::make_unique<Socket>(sockfd))
    , channel_(std::make_unique<Channel>(loop, sockfd))
    , local_addr_(local_addr)
    , peer_addr_(peer_addr)
    , high_water_mark_(64 * 1024 * 1024) {
    // 设置连接上相应的回调函数
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));

    LOGGER_DEBUG("net", "Create TcpConnection {} at {}, fd: {}", name_, fmt::ptr(this), sockfd);
    socket_->enableKeepAlive();
}

TcpConnection::~TcpConnection() {
    LOGGER_DEBUG("net", "Destory TcpConnection {} at {}, fd: {}, state: {}", name_,
        fmt::ptr(this), channel_->fd(), toString(state_));
    assert(state_ == State::Disconnected);
}

EventLoop* TcpConnection::loop() const {
    return loop_;
}

const std::string& TcpConnection::name() const {
    return name_;
}

const InetAddress& TcpConnection::localAddress() const {
    return local_addr_;
}

const InetAddress& TcpConnection::peerAddress() const {
    return peer_addr_;
}

void TcpConnection::send(std::string_view message) {
    if (state_ == State::Connected) {
        if (loop_->isInCreatorThread()) {
            send_(message);
        } else {
            loop_->runInLoop(std::bind(&TcpConnection::send_, this, message));
        }
    }
}

void TcpConnection::send(ByteBuffer* buffer) {
    if (state_ == State::Connected) {
        std::string message;
        buffer->readBytes(message);

        if (loop_->isInCreatorThread()) {
            send_(message);
        } else {
            loop_->runInLoop(std::bind(&TcpConnection::send_, this, message));
        }
    }
}

bool TcpConnection::connected() const {
    return state_ == State::Connected;
}

bool TcpConnection::disconnected() const {
    return state_ == State::Disconnected;
}

void TcpConnection::shutdown() {
    if (state_ == State::Connected) {
        state_ = State::Disconnecting;
        loop_->runInLoop(std::bind(&TcpConnection::shutdown_, this));
    }
}

void TcpConnection::forceClose() {
    if (state_ == State::Connected || state_ == State::Disconnecting) {
        state_ = State::Disconnecting;
        loop_->queueInLoop(std::bind(&TcpConnection::forceClose_, shared_from_this()));
    }
}

void TcpConnection::startRead() {
    loop_->runInLoop(std::bind(&TcpConnection::startRead_, this));
}

void TcpConnection::stopRead() {
    loop_->runInLoop(std::bind(&TcpConnection::stopRead_, this));
}

void TcpConnection::enabelTcpNoDelay(bool enabled) {
    socket_->enableTcpNoDelay(enabled);
}

void TcpConnection::setConnectionCallback(ConnectionCallback cb) {
    connection_cb_ = std::move(cb);
}

void TcpConnection::setMessageCallback(MessageCallback cb) {
    message_cb_ = std::move(cb);
}

void TcpConnection::setWriteCompleteCallback(WriteCompleteCallback cb) {
    write_complete_cb_ = std::move(cb);
}

void TcpConnection::setHighWaterMarkCallback(HighWaterMarkCallback cb, size_t high_water_mark) {
    high_water_mark_cb_ = std::move(cb);
    high_water_mark_    = high_water_mark;
}

void TcpConnection::setCloseCallback(CloseCallback cb) {
    close_cb_ = std::move(cb);
}

ByteBuffer* TcpConnection::inputBuffer() {
    return &input_buffer_;
}

ByteBuffer* TcpConnection::outputBuffer() {
    return &output_buffer_;
}

void TcpConnection::connectionEstablished() {
    loop_->checkIsInCreatorThread();
    assert(state_ == State::Connecting);
    state_ = State::Connected;
    channel_->ensureAlive(shared_from_this());
    channel_->enableReading();

    connection_cb_(shared_from_this());
}

void TcpConnection::connectionDestoryed() {
    loop_->checkIsInCreatorThread();
    if (state_ == State::Connected) {
        state_ = State::Disconnected;
        channel_->disableAll();

        connection_cb_(shared_from_this());
    }
    channel_->remove();
}

void TcpConnection::handleRead(TimePoint receive_time) {
    loop_->checkIsInCreatorThread();
    int saved_errno = 0;

    ssize_t n = input_buffer_.recvFromFd(channel_->fd(), &saved_errno);

    if (n > 0) {
        message_cb_(shared_from_this(), &input_buffer_, receive_time);
    } else if (n == 0) {
        handleClose();
    } else {
        LOGGER_ERROR("net", "Handle read event error on {}: {}", name_, std::strerror(saved_errno));
        handleError();
    }
}

void TcpConnection::handleWrite() {
    loop_->checkIsInCreatorThread();
    if (channel_->isWriting()) {
        ssize_t n = common::write(channel_->fd(), output_buffer_.writerPtr(), output_buffer_.readableBytes());

        int saved_errno = errno;

        if (n > 0) { // 写入成功
            output_buffer_.skipBytes(n);
            if (!output_buffer_.isReadable()) {
                // 如果输出缓冲区的数据写入完成 则关闭写事件 并执行写操作完成回调函数
                channel_->enableWriting(false);
                if (write_complete_cb_) {
                    loop_->queueInLoop(std::bind(write_complete_cb_, shared_from_this()));
                }
                // 如果此时正处于关闭连接状态 则关闭写端
                if (state_ == State::Disconnecting) {
                    shutdown_();
                }
            }
        } else { // 写入失败
            LOGGER_ERROR("net", "Handle write event error on {}: {}", name_, strerror(saved_errno));
        }
    }
}

void TcpConnection::handleClose() {
    loop_->checkIsInCreatorThread();
    LOGGER_TRACE("net", "Handle close event on fd {}, state: {}", channel_->fd(), toString(state_));
    assert(state_ == State::Connected || state_ == State::Disconnecting);
    state_ = State::Disconnected;
    channel_->disableAll();

    TcpConnectionPtr tmp(shared_from_this());
    connection_cb_(tmp);
    close_cb_(tmp);
}

void TcpConnection::handleError() {
    int err = common::getSocketError(channel_->fd());
    LOGGER_ERROR("net", "Handle error from {}: {}", name_, std::strerror(err));
}

void TcpConnection::send_(std::string_view message) {
    loop_->checkIsInCreatorThread();

    ssize_t nwrote    = 0;              // 已经写入的字节数目
    size_t  remaining = message.size(); // 剩余的字节数目
    bool    fault     = false;
    if (state_ == State::Disconnected) {
        LOGGER_WARN("net", "Disconnected, give up writing");
        return;
    }

    // 如果输出缓冲区中没有数据 尝试直接发送数据
    if (!channel_->isWriting() && !output_buffer_.isReadable()) {
        nwrote = common::write(channel_->fd(), message.data(), message.size());
        if (nwrote >= 0) { // 写入正确
            remaining -= nwrote;
            if (remaining == 0 && write_complete_cb_) {
                loop_->queueInLoop(std::bind(write_complete_cb_, shared_from_this()));
            }
        } else { // 写入错误
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LOGGER_ERROR("net", "Failed to write data to peer {} from {}", peer_addr_.toIpPort(), name_);
                // EPIPE 意味着与另一端的通信已经中断或对方已经关闭了连接
                // ECONNRESET 表示连接已经被重置，通常需要重新建立连接或重新发送数据
                if (errno == EPIPE || errno == ECONNRESET) {
                    fault = true;
                }
            }
        }
    }

    assert(remaining <= message.size());
    // 如果没有发生EPIPE和ECONNRESET错误 且还有剩余数据未发送
    if (!fault && remaining > 0) {
        size_t old_len = output_buffer_.readableBytes();
        // 如果缓冲区数据达到高水位标记则调用相应的回调函数
        if (old_len + remaining >= high_water_mark_ && old_len < high_water_mark_ && high_water_mark_cb_) {
            loop_->queueInLoop(std::bind(high_water_mark_cb_, shared_from_this(), old_len + remaining));
        }
        // 将剩余字节写入到输出缓冲区中
        output_buffer_.writeBytes(std::string_view(message.data() + nwrote, remaining));

        // 让描述符关注可写事件
        if (!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::shutdown_() {
    loop_->checkIsInCreatorThread();
    if (!channel_->isWriting()) {
        // 关闭写端
        socket_->shutdownWrite();
    }
}

void TcpConnection::forceClose_() {
    loop_->checkIsInCreatorThread();
    if (state_ == State::Connected || state_ == State::Disconnecting) {
        handleClose();
    }
}

void TcpConnection::startRead_() {
    loop_->checkIsInCreatorThread();
    if (!reading_ || !channel_->isReading()) {
        channel_->enableReading();
        reading_ = true;
    }
}

void TcpConnection::stopRead_() {
    loop_->checkIsInCreatorThread();
    if (reading_ || channel_->isReading()) {
        channel_->enableReading(false);
        reading_ = false;
    }
}

std::string TcpConnection::toString(State state) {
    switch (state) {
    case State::Connected:
        return "Connected";
    case State::Disconnected:
        return "Disconnected";
    case State::Connecting:
        return "Connecting";
    case State::Disconnecting:
        return "Disconnecting";
    default:
        assert(0);
        return "Unknonw";
    }
}
} // namespace talko::net