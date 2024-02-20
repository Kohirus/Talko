#include <log/log.h>
#include <net/common.h>
#include <net/socket.h>
#include <netinet/tcp.h>

namespace talko::net {
Socket::Socket(int sockfd)
    : sockfd_(sockfd) {
}

Socket::~Socket() {
    common::close(sockfd_);
}

int Socket::fd() const {
    return sockfd_;
}

void Socket::bindAddress(const InetAddress& localAddr) {
    common::bind(sockfd_, localAddr.sockAddr());
    LOG_DEBUG("Bind address {}", localAddr.toIpPort());
}

void Socket::listen() {
    common::listen(sockfd_);
}

int Socket::accept(InetAddress& peeraddr) {
    sockaddr_in addr;
    ::bzero(&addr, sizeof(addr));
    int connfd = common::accept(sockfd_, reinterpret_cast<sockaddr*>(&addr));
    peeraddr.setSockAddr(addr);
    return connfd;
}

void Socket::shutdownWrite() {
    common::shutdownWrite(sockfd_);
}

void Socket::enableTcpNoDelay(bool enabled) {
    common::enableSocketOption(sockfd_, IPPROTO_TCP, TCP_NODELAY, enabled);
}

void Socket::enableReuseAddr(bool enabled) {
    common::enableSocketOption(sockfd_, SOL_SOCKET, SO_REUSEADDR, enabled);
}

void Socket::enableReusePort(bool enabled) {
    common::enableSocketOption(sockfd_, SOL_SOCKET, SO_REUSEPORT, enabled);
}

void Socket::enableKeepAlive(bool enabled) {
    common::enableSocketOption(sockfd_, SOL_SOCKET, SO_KEEPALIVE, enabled);
}
} // namespace talko::net