#include <log/log.h>
#include <net/common.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/uio.h>
#include <unistd.h>
#include <utils/datetime.h>

namespace talko::net::common {
void setSocketAddr(sockaddr_in& addr, std::string_view ip, uint16_t port) {
    addr.sin_family = AF_INET;
    addr.sin_port   = hostToNetwork(port);
    if (::inet_pton(AF_INET, ip.data(), &addr.sin_addr) < 0) {
        LOGGER_FATAL("net", "Failed to set sockaddr_in [{}:{}]: {}", ip, port, std::strerror(errno));
    }
}

sockaddr_in getLocalAddr(int sockfd) {
    sockaddr_in local_addr;
    ::bzero(&local_addr, sizeof(local_addr));
    socklen_t addr_len = static_cast<socklen_t>(sizeof(local_addr));
    if (::getsockname(sockfd, reinterpret_cast<sockaddr*>(&local_addr), &addr_len) < 0) {
        LOGGER_ERROR("net", "Failed to get local address: {}", std::strerror(errno));
    }
    return local_addr;
}

sockaddr_in getPeerAddr(int sockfd) {
    sockaddr_in peer_addr;
    ::bzero(&peer_addr, sizeof(peer_addr));
    socklen_t addr_len = static_cast<socklen_t>(sizeof(peer_addr));
    if (::getpeername(sockfd, reinterpret_cast<sockaddr*>(&peer_addr), &addr_len) < 0) {
        LOGGER_ERROR("net", "Failed to get peer address: {}", std::strerror(errno));
    }
    return peer_addr;
}

int getSocketError(int sockfd) {
    int       opt_val;
    socklen_t opt_len = static_cast<socklen_t>(sizeof(opt_val));

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &opt_val, &opt_len) < 0) {
        return errno;
    } else {
        return opt_val;
    }
}

std::string toIp(const sockaddr_in& addr) {
    char ip[INET_ADDRSTRLEN];
    ::inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
    return ip;
}

std::string toIpPort(const sockaddr_in& addr) {
    std::string res  = toIp(addr);
    uint16_t    port = networkToHost(addr.sin_port);
    res.push_back(':');
    res.append(std::to_string(port));
    return res;
}

void close(int sockfd) {
    if (::close(sockfd) < 0) {
        LOGGER_ERROR("net", "Failed to close fd {}: {}", sockfd, std::strerror(errno));
    }
}

int createNonblockingSocket(sa_family_t family) {
    int sockfd = socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0) {
        LOGGER_FATAL("net", "Failed to create socket: {}", std::strerror(errno));
    }
    LOGGER_TRACE("net", "Create socket fd {}", sockfd);
    return sockfd;
}

int createEventFd() {
    int evt_fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evt_fd < 0) {
        LOGGER_FATAL("net", "Failed to create eventfd: {}", std::strerror(errno));
    }
    LOGGER_TRACE("net", "Create event fd {}", evt_fd);
    return evt_fd;
}

int createTimerFd() {
    int timer_fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timer_fd < 0) {
        LOGGER_FATAL("net", "Failed to create timerfd: {}", std::strerror(errno));
    }
    LOGGER_TRACE("net", "Create timer fd {}", timer_fd);
    return timer_fd;
}

void resetTimerFd(int timer_fd, TimePoint expiration) {
    itimerspec new_val, old_val;
    ::bzero(&new_val, sizeof(new_val));
    ::bzero(&old_val, sizeof(old_val));

    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::nanoseconds;
    using std::chrono::seconds;

    std::timespec ts;
    TimePoint     now = high_resolution_clock::now();

    ts.tv_sec  = duration_cast<seconds>(expiration - now).count();
    ts.tv_nsec = duration_cast<nanoseconds>(expiration - now).count() % 1000000000;
    // 触发过快时 expiration会小于now产生负数
    if (ts.tv_sec < 0) ts.tv_sec = 0;
    if (ts.tv_nsec < 0) ts.tv_nsec = 100;

    new_val.it_value = ts;

    int ret = ::timerfd_settime(timer_fd, 0, &new_val, &old_val);
    if (ret) {
        LOGGER_FATAL("net", "Failed to set timerfd {}: {}", timer_fd, std::strerror(errno));
    }
}

void readTimerFd(int timer_fd, TimePoint now) {
    uint64_t howmany;
    ::read(timer_fd, &howmany, sizeof(howmany));
    LOGGER_TRACE("net", "Read {} bytes from timerfd {}", howmany, timer_fd);
}

int createEpoll() {
    int epfd = epoll_create1(EPOLL_CLOEXEC);
    if (epfd < 0) {
        LOGGER_FATAL("net", "Failed to create epoll: {}", std::strerror(errno));
    }
    LOGGER_TRACE("net", "Create epoll fd {}", epfd);
    return epfd;
}

void bind(int sockfd, const sockaddr* addr) {
    if (::bind(sockfd, addr, static_cast<socklen_t>(sizeof(sockaddr_in))) < 0) {
        LOGGER_FATAL("net", "Failed to bind socket fd {}: {}", sockfd, std::strerror(errno));
    }
}

void listen(int sockfd) {
    if (::listen(sockfd, SOMAXCONN) < 0) {
        LOGGER_FATAL("net", "Failed to listen socket fd {}: {}", sockfd, std::strerror(errno));
    }
}

int accept(int sockfd, sockaddr* addr) {
    socklen_t addr_len = static_cast<socklen_t>(sizeof(*addr));
    // 设置新创建的套接字为非阻塞模式 且在执行 exec 时自动关闭
    int connfd = ::accept4(sockfd, addr, &addr_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        LOGGER_ERROR("net", "Failed to accept socket fd {}: {}", sockfd, std::strerror(errno));
    }
    return connfd;
}

int connect(int sockfd, const sockaddr* addr) {
    return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(sockaddr_in)));
}

void enableSocketOption(int sockfd, int level, int optname, bool enabled) {
    int optval = enabled ? 1 : 0;
    if (::setsockopt(sockfd, level, optname, &optval, static_cast<socklen_t>(sizeof(optval))) < 0) {
        LOGGER_ERROR("net", "Failed to {} socket option {}: {}", enabled ? "enable" : "disable",
            optname, std::strerror(errno));
    }
}

void shutdownWrite(int sockfd) {
    if (::shutdown(sockfd, SHUT_WR) < 0) {
        LOGGER_ERROR("net", "Failed to shutdown write of {}: {}", sockfd, std::strerror(errno));
    }
}

ssize_t write(int sockfd, const void* buf, size_t count) {
    return ::write(sockfd, buf, count);
}

ssize_t read(int sockfd, void* buf, size_t count) {
    return ::read(sockfd, buf, count);
}

ssize_t readv(int sockfd, const iovec* iov, int iovcnt) {
    return ::readv(sockfd, iov, iovcnt);
}

bool isSelfConnection(int sockfd) {
    sockaddr_in local_addr = getLocalAddr(sockfd);
    sockaddr_in peer_addr  = getPeerAddr(sockfd);
    if (local_addr.sin_family == AF_INET) {
        return local_addr.sin_port == peer_addr.sin_port
            && local_addr.sin_addr.s_addr == peer_addr.sin_addr.s_addr;
    }
    return false;
}

std::string eventsToString(int events) {
    std::string res = "";
    if (events & POLLIN) res.append("IN|");
    if (events & POLLPRI) res.append("PRI|");
    if (events & POLLOUT) res.append("OUT|");
    if (events & POLLHUP) res.append("HUP|");
    if (events & POLLRDHUP) res.append("RDHUP|");
    if (events & POLLERR) res.append("ERR|");
    if (events & POLLNVAL) res.append("NVAL|");
    if (res.empty()) {
        return "None";
    }
    res.pop_back();
    return res;
}

std::string operationToString(int operation) {
    switch (operation) {
    case EPOLL_CTL_ADD:
        return "Add";
    case EPOLL_CTL_DEL:
        return "Del";
    case EPOLL_CTL_MOD:
        return "Mod";
    default:
        assert(false && "Unknown operation type");
        return "Unknown";
    }
}
} // namespace talko::net::common