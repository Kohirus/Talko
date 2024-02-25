#include <rpc/rpc_application.h>
#include <rpc/rpc_provider.h>

namespace talko::rpc {
void RpcProvider::run() {
    net::TcpServer server(&loop_, RpcApplication::instance().localAddress(),
        RpcApplication::instance().serverName(), RpcApplication::instance().reusePort());

    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::onMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3));
}

void RpcProvider::onConnection(const net::TcpConnectionPtr& conn) {
}

void RpcProvider::onMessage(const net::TcpConnectionPtr& conn,
    net::ByteBuffer* buffer, net::TimePoint time) {
}
} // namespace talko::rpc