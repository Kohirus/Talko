#include <net/net.h>
using namespace talko;

class Client {
public:
    Client(const net::InetAddress& server_addr, const std::string& name)
        : client_(&loop_, server_addr, name) {
        client_.enableRetry();
        client_.setConnectionCallback(std::bind(&Client::connectionCallback, this, std::placeholders::_1));
        client_.setMessageCallback(std::bind(&Client::messageCallback, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3));
    }

    void start() {
        client_.connect();
        loop_.loop();
    }

private:
    void connectionCallback(const net::TcpConnectionPtr& conn) {
        if (conn->connected()) {
            log::info("Connection established");
            timer_id_ = loop_.runEvery(std::chrono::seconds(2), [&]() {
                client_.connection()->send("Hello, Server!");
            });
        } else {
            log::info("Connection destoryed");
            loop_.cancel(timer_id_);
        }
    }

    void messageCallback(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time) {
        std::string message;
        buffer->readBytes(message);
        log::info("Receive server message: {}", message);
    }

private:
    net::EventLoop loop_;
    net::TcpClient client_;
    net::TimerId   timer_id_;
};

int main() {
    auto logger = log::createConsoleLoggerMt("net");
    log::registerLogger(logger);
    log::setGlobalLevel(log::LogLevel::debug);
    log::setPattern("[%T.%f] [%C] [%l] [%E] %v");

    net::InetAddress server_addr(8888);

    Client client(server_addr, "client");

    client.start();

    return 0;
}