#include <log/log.h>
#include <net/acceptor.h>
#include <net/common.h>
#include <net/event_loop.h>

namespace talko::net {
Acceptor::Acceptor(EventLoop* loop, const InetAddress& listen_addr, bool resuse_port)
    : loop_(loop)
    , accept_socket_(common::createNonblockingSocket(listen_addr.family()))
    , accept_channel_(loop, accept_socket_.fd())
    , listening_(false) {
    accept_socket_.enableReuseAddr();
    accept_socket_.enableReusePort(resuse_port);
    accept_socket_.bindAddress(listen_addr);
    accept_channel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    accept_channel_.disableAll();
    accept_channel_.remove();
}

void Acceptor::setNewConnectionCallback(NewConnectionCallback cb) {
    new_connection_cb_ = std::move(cb);
}

bool Acceptor::listening() const {
    return listening_;
}

void Acceptor::listen() {
    loop_->checkIsInCreatorThread();
    listening_ = true;
    accept_socket_.listen();
    accept_channel_.enableReading();
}

void Acceptor::handleRead() {
    loop_->checkIsInCreatorThread();
    InetAddress peer_addr;

    int connfd = accept_socket_.accept(peer_addr);
    if (connfd >= 0) {
        LOG_TRACE("Accept new client {} on fd {}", peer_addr.toIpPort(), connfd);
        if (new_connection_cb_) {
            new_connection_cb_(connfd, peer_addr);
        } else {
            common::close(connfd);
        }
    }
}
} // namespace talko::net