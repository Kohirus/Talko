#include <log/log.h>
#include <net/callbacks.h>
#include <net/tcp_connection.h>

namespace talko::net {
/** 默认的连接回调函数 */
void defaultConnectionCallback(const TcpConnectionPtr& conn) {
    LOGGER_TRACE("net", "Connection between {} and {} is {}", conn->localAddress().toIpPort(),
        conn->peerAddress().toIpPort(), (conn->connected() ? "up" : "down"));
}

/** 默认的消息回调函数 */
void defaultMessageCallback(const TcpConnectionPtr&, ByteBuffer* buffer, TimePoint) {
    buffer->skipAllBytes();
}
} // namespace talko::net