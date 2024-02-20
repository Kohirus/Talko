#pragma once

#include <functional>
#include <net/channel.h>
#include <net/inet_address.h>
#include <net/socket.h>

namespace talko::net {
class EventLoop;

/**
 * @brief 连接接收类
 *
 */
class Acceptor {
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;

    /**
     * @brief Construct a new Accepter object
     *
     * @param loop 事件循环
     * @param listen_addr 监听地址
     * @param resuse_port 是否复用端口
     */
    Acceptor(EventLoop* loop, const InetAddress& listen_addr, bool resuse_port);
    ~Acceptor();

    Acceptor(const Acceptor&)            = delete;
    Acceptor& operator=(const Acceptor&) = delete;

    Acceptor(Acceptor&&)            = delete;
    Acceptor& operator=(Acceptor&&) = delete;

    /** 设置新连接回调函数 */
    void setNewConnectionCallback(NewConnectionCallback cb);

    /** 开始监听 */
    void listen();

    /** 是否正在监听 */
    bool listening() const;

private:
    /** 处理套接字上的可读事件 */
    void handleRead();

private:
    EventLoop* loop_;           ///< 事件循环
    Socket     accept_socket_;  ///< 监听套接字
    Channel    accept_channel_; ///< 监听套接字通道
    bool       listening_;      ///< 是否正在监听

    NewConnectionCallback new_connection_cb_; ///< 新连接回调函数
};
} // namespace talko::net