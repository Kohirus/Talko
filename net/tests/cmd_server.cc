#include <net/net.h>
#include <thread>
using namespace talko;

class CmdServer {
public:
    CmdServer(net::EventLoop* loop, const net::InetAddress& listen_addr)
        : loop_(loop)
        , server_(loop, listen_addr, "cmd") {
        server_.setConnectionCallback(std::bind(&CmdServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(std::bind(&CmdServer::onMessage, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3));
    }

    void start() {
        server_.start();
    }

private:
    void onConnection(const net::TcpConnectionPtr& conn) {
        if (conn->connected()) {
            LOGGER_INFO("net", "cmd_server", "{} connect", conn->peerAddress().toIpPort());
            conn->send("Command Format: {[number]|stop|close}\n");
            timers_[conn->name()] = std::nullopt;
        } else {
            LOGGER_INFO("net", "cmd_server", "{} disconnect", conn->peerAddress().toIpPort());
            auto it = timers_.find(conn->name());
            if (it != timers_.end() && it->second.has_value()) {
                loop_->cancel(it->second.value());
                it->second.reset();
                timers_.erase(it);
            }
        }
    }

    void onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time) {
        std::string message;
        buffer->readBytes(message);
        message.pop_back();

        if (message == "stop") {
            auto it = timers_.find(conn->name());
            if (it != timers_.end() && it->second.has_value()) {
                loop_->cancel(it->second.value());
                it->second.reset();
                timers_.erase(it);
            }
            conn->send("Timer is stop\n");
        } else if (message == "close") {
            conn->forceClose();
        } else {
            int ms = 0;
            try {
                ms = std::stoi(message);
            } catch (std::exception e) {
                conn->send("Unexpected command\n");
                return;
            }
            conn->send("Timer is start\n");
            auto timer_id         = loop_->runEvery(std::chrono::milliseconds(ms), std::bind(&CmdServer::sendHello, this, conn));
            timers_[conn->name()] = timer_id;
        }
    }

    void sendHello(const net::TcpConnectionPtr& conn) {
        if (conn->connected()) {
            conn->send("Hello\n");
        }
    }

private:
    net::EventLoop* loop_;
    net::TcpServer  server_;

    std::map<std::string, std::optional<net::TimerId>> timers_;
};

int main() {
    log::defaultLogger()->setLevel(log::LogLevel::debug);
    log::defaultLogger()->setPattern("[%T.%f] [%C] [%l] [%E] %v");

    auto logger = log::createConsoleLoggerMt("cmd_server");
    logger->setLevel(log::LogLevel::debug);
    log::registerLogger(logger);

    tp::setThreadPoolMode(tp::ThreadPoolMode::dynamic);
    tp::start();

    net::EventLoop loop;

    net::InetAddress local_addr(8888);
    CmdServer        server(&loop, local_addr);

    server.start();
    loop.loop();

    return 0;
}