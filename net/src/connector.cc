#include <log/log.h>
#include <net/channel.h>
#include <net/common.h>
#include <net/connector.h>
#include <net/event_loop.h>
#include <random>

namespace talko::net {
/**
 * @brief 获取随机初始化重连时间
 * @details 如果服务器宕机，所有的客户端连接同时断开，如果在相同的时间延迟后
 * 所有的客户端同时发起连接请求，既可能造成SYN丢包，也可能给服务器带来短期的大
 * 负载，影响服务质量。因此每个客户端的初始化重连时间应为随机时间，避免拥塞。
 *
 * @param min_delay 最小初始化重连时间
 * @param max_delay 最大初始化重连时间
 * @return int 返回初始化重连时间
 */
int randomInitRetryDelay(int min_delay, int max_delay) {
    std::random_device                 rd;
    std::mt19937                       gen(rd());
    std::uniform_int_distribution<int> dis(min_delay, max_delay);
    return dis(gen);
}

const int Connector::kMaxRetryDelay { 30 * 1000 };
const int Connector::kMinInitRetryDelay { 500 };
const int Connector::kMaxInitRetryDelay { 2000 };

Connector::Connector(EventLoop* loop, const InetAddress& server_addr)
    : loop_(loop)
    , server_addr_(server_addr)
    , retry_delay_(randomInitRetryDelay(kMinInitRetryDelay, kMaxInitRetryDelay)) {
    LOGGER_DEBUG("net", "Create Connecter[{}]", fmt::ptr(this));
}

Connector::~Connector() {
    LOGGER_DEBUG("net", "Desotry Connecter[{}]", fmt::ptr(this));
    assert(!channel_);
}

void Connector::setNewConnectionCallback(NewConnectionCallback cb) {
    new_connection_cb_ = std::move(cb);
}

void Connector::start() {
    connect_ = true;
    loop_->runInLoop(std::bind(&Connector::start_, this));
}

void Connector::restart() {
    loop_->checkIsInCreatorThread();
    state_       = State::Disconnected;
    retry_delay_ = randomInitRetryDelay(kMinInitRetryDelay, kMaxInitRetryDelay);
    connect_     = true;
    start_();
}

void Connector::stop() {
    connect_ = false;
    loop_->queueInLoop(std::bind(&Connector::stop_, this));
}

const InetAddress& Connector::serverAddress() const {
    return server_addr_;
}

void Connector::start_() {
    loop_->checkIsInCreatorThread();
    assert(state_ == State::Disconnected);
    if (connect_) {
        connect();
    } else {
        LOGGER_DEBUG("net", "do not connect");
    }
}

void Connector::stop_() {
    loop_->checkIsInCreatorThread();
    if (state_ == State::Connecting) {
        state_     = State::Disconnected;
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void Connector::connect() {
    int sockfd = common::createNonblockingSocket(server_addr_.family());
    int ret    = common::connect(sockfd, server_addr_.sockAddr());

    int save_errno = (ret == 0) ? 0 : errno;
    switch (save_errno) {
    case 0:
    case EINPROGRESS: // 表示一个非阻塞套接字连接正在进行中
    case EINTR:       // 表示一个系统调用被信号中断
    case EISCONN:     // 表示一个套接字已经连接
        connecting(sockfd);
        break;
    case EAGAIN:        // 表示操作将被阻塞，在非阻塞 I/O 操作中，当没有可用数据或无法立即完成操作时，可能会返回这个错误码
    case EADDRINUSE:    // 表示地址已经在使用中
    case EADDRNOTAVAIL: // 表示请求的地址不可用
    case ECONNREFUSED:  // 表示连接被拒绝，当尝试连接到一个未监听或不可达的地址时，会返回这个错误码
    case ENETUNREACH:   // 表示网络不可达，当尝试连接到一个不可达的网络或主机时，会返回这个错误码
        retry(sockfd);
        break;
    case EACCES:       // 表示权限不足
    case EPERM:        // 表示操作被拒绝
    case EAFNOSUPPORT: // 表示地址族不支持
    case EALREADY:     // 表示操作已经在进行中，在一些非阻塞操作中，当尝试执行一个操作，但是该操作已经在进行中时，会返回这个错误码
    case EBADF:        // 表示文件描述符无效
    case EFAULT:       // 表示地址无效，在一些系统调用中，当尝试使用一个无效的内存地址时，会返回这个错误码
    case ENOTSOCK:     // 表示不是一个套接字
        LOGGER_ERROR("net", "Failed to connect: {}", std::strerror(save_errno));
        common::close(sockfd);
        break;
    default:
        LOGGER_ERROR("net", "Unexpected error when connect: {}", std::strerror(save_errno));
        common::close(sockfd);
        break;
    }
}

void Connector::connecting(int sockfd) {
    state_ = State::Connecting;
    assert(!channel_);
    channel_ = std::make_unique<Channel>(loop_, sockfd);
    // 对于非阻塞connect 当连接建立成功时套接字可写 当连接建立失败时套接字既可读又可写
    channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
    channel_->setErrorCallback(std::bind(&Connector::handleError, this));
    channel_->enableWriting();
}

void Connector::retry(int sockfd) {
    common::close(sockfd);
    state_ = State::Disconnected;
    if (connect_) {
        LOGGER_INFO("net", "Retry to connect with {} in {} ms", server_addr_.toIpPort(),
            retry_delay_);
        loop_->runAfter(std::chrono::milliseconds(retry_delay_), std::bind(&Connector::start_, this));
        // 逐渐延长重连时间
        retry_delay_ = std::min(retry_delay_ * 2, kMaxRetryDelay);
    } else {
        LOGGER_DEBUG("net", "do not connect");
    }
}

int Connector::removeAndResetChannel() {
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->fd();
    // 不能重置channel_ 因为此时正在处理套接字上的可写或错误事件
    loop_->queueInLoop(std::bind(&Connector::resetChannel, this));
    return sockfd;
}

void Connector::resetChannel() {
    channel_.reset();
}

void Connector::handleWrite() {
    LOGGER_TRACE("net", "Handle write event, current state: {}", toString(state_));

    if (state_ == State::Connecting) {
        int sockfd = removeAndResetChannel();
        // 获取套接字上的待处理错误 如果连接建立成功则会返回0 如果连接发送错误则返回errno
        int err = common::getSocketError(sockfd);
        if (err) {
            LOGGER_WARN("net", "Happen error when connect: {}", std::strerror(err));
            retry(sockfd);
        } else if (common::isSelfConnection(sockfd)) {
            // 一个TCP连接由四元组(源IP、源端口、目的IP、目的端口)组成
            // 其中，源IP、目的IP、目的端口都是确定的 唯一不确定的是源端口
            // 当服务器和客户端处于同一个网络设备上时
            // 如果此时服务端还未启动或者服务端异常挂掉了 同时启动客户端
            // 而系统选择的源端口与目的端口相应 则此时就是相同的TCP实体
            LOGGER_WARN("net", "Self connection");
            retry(sockfd);
        } else {
            state_ = State::Connected;
            if (connect_) {
                new_connection_cb_(sockfd);
            } else {
                common::close(sockfd);
            }
        }
    } else {
        assert(state_ == State::Disconnected);
    }
}

void Connector::handleError() {
    LOGGER_ERROR("net", "Handle error event, current state: {}", toString(state_));
    if (state_ == State::Connecting) {
        int sockfd = removeAndResetChannel();
        int err    = common::getSocketError(sockfd);
        LOGGER_TRACE("net", "Happen error when connect: {}", std::strerror(err));
        retry(sockfd);
    }
}

std::string Connector::toString(State state) {
    switch (state) {
    case State::Connected:
        return "connected";
    case State::Disconnected:
        return "disconnected";
    case State::Connecting:
        return "connecting";
    default:
        assert(0);
        return "unknown";
    }
}
} // namespace talko::net