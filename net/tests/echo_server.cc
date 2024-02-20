#include <log/log.h>
#include <net/event_loop.h>
#include <net/inet_address.h>
#include <net/tcp_connection.h>
#include <net/tcp_server.h>
using namespace talko;

void connectionCallback(const net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        log::get("echo_svr")->debug("Connection {} Established", conn->name());
    } else {
        log::get("echo_svr")->debug("Connection {} Destoryed", conn->name());
    }
}

void echoMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint receive_time) {
    std::string message = "";
    buffer->readBytes(message);
    log::get("echo_svr")->debug("Receive message from connection {}: {}", conn->name(), message);
    conn->send(message);
}

void writeCompleteCallback(const net::TcpConnectionPtr& conn) {
    log::get("echo_svr")->debug("Echo message to {} in connection completely", conn->peerAddress().toIpPort(), conn->name());
}

int main() {
    log::defaultLogger()->setLevel(log::LogLevel::debug);
    log::defaultLogger()->setPattern("[%T.%f] [%C] [%l] [%E] %v");
    // log::defaultLogger()->setLevel(log::LogLevel::trace);
    // log::defaultLogger()->setPattern("[%T.%f] [%C] [%l] [%E] [%k] %v");

    auto logger = log::createConsoleLoggerMt("echo_svr");
    logger->setLevel(log::LogLevel::debug);
    log::registerLogger(logger);

    tp::setThreadPoolMode(tp::ThreadPoolMode::dynamic);
    tp::start();

    net::EventLoop   loop;
    net::InetAddress listen_addr(8888);
    net::TcpServer   server(&loop, listen_addr, "echo");

    server.setConnectionCallback(connectionCallback);
    server.setWriteCompleteCallback(writeCompleteCallback);
    server.setMessageCallback(echoMessage);

    server.start();
    loop.loop();

    return 0;
}