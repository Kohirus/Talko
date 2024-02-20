#pragma once

#include <net/inet_address.h>

namespace talko::net {
/**
 * @brief 套接字
 * @details 使用RAII方法绑定套接字描述符，对象析构时自动关闭套接字描述符
 */
class Socket {
public:
    /**
     * @brief Construct a new Socket object
     *
     * @param sockfd 套接字描述符
     */
    explicit Socket(int sockfd);
    ~Socket();

    Socket(const Socket&)            = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&&)            = delete;
    Socket& operator=(Socket&&) = delete;

    /** 获取套接字描述符 */
    int fd() const;

    /** 绑定网络地址 */
    void bindAddress(const InetAddress& localAddr);

    /** 监听客户端连接到来 */
    void listen();

    /** 接收客户端连接，返回相应的套接字描述符 */
    int accept(InetAddress& peeraddr);

    /** 关闭写通道 */
    void shutdownWrite();

    /** 开启 TCP_NODELAY 选项 */
    void enableTcpNoDelay(bool enabled = true);

    /** 开启地址复用 */
    void enableReuseAddr(bool enabled = true);

    /** 开启端口复用 */
    void enableReusePort(bool enable = true);

    /** 开启 SO_KEEPALIVE 选项 */
    void enableKeepAlive(bool enable = true);

private:
    const int sockfd_; ///< 套接字描述符
};
} // namespace talko::net