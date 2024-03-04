#pragma once

#include <any>
#include <atomic>
#include <memory>
#include <net/byte_buffer.h>
#include <net/callbacks.h>
#include <net/inet_address.h>
#include <string>

namespace talko::net {
class EventLoop;
class Socket;
class Channel;

/**
 * @brief TCP连接类
 *
 */
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    /**
     * @brief Construct a new Tcp Connection object
     *
     * @param loop 事件循环
     * @param name 连接名称
     * @param sockfd 连接套接字
     * @param local_addr 本机地址
     * @param peer_addr 对端地址
     */
    TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
        const InetAddress& local_addr, const InetAddress& peer_addr);
    ~TcpConnection();

    /** 获取事件循环 */
    EventLoop* loop() const;

    /** 获取连接名称 */
    const std::string& name() const;

    /** 获取本机地址 */
    const InetAddress& localAddress() const;

    /** 获取对端地址 */
    const InetAddress& peerAddress() const;

    /** 向对端发送消息 */
    void send(const std::string& message);

    /** 将缓冲区中的可读数据发送给对端 */
    void send(ByteBuffer* buffer);

    /** 是否已建立连接 */
    bool connected() const;

    /** 是否已断开连接 */
    bool disconnected() const;

    /** 关闭连接 */
    void shutdown();

    /** 强制关闭 */
    void forceClose();

    /** 开始读数据 */
    void startRead();

    /** 停止读数据 */
    void stopRead();

    /** 开启TCP NO_DELAY选项 */
    void enabelTcpNoDelay(bool enabled = true);

    /** 设置连接回调函数 */
    void setConnectionCallback(ConnectionCallback cb);

    /** 设置消息回调函数 */
    void setMessageCallback(MessageCallback cb);

    /** 设置写事件完成回调函数 */
    void setWriteCompleteCallback(WriteCompleteCallback cb);

    /** 设置高水位标记回调函数 */
    void setHighWaterMarkCallback(HighWaterMarkCallback cb, size_t high_water_mark);

    /** 设置关闭回调函数 */
    void setCloseCallback(CloseCallback cb);

    /** 获取输入缓冲区 */
    ByteBuffer* inputBuffer();

    /** 获取输出缓冲区 */
    ByteBuffer* outputBuffer();

    /** 当TcpServer接收一个新的连接时调用 */
    void connectionEstablished();

    /** 当TcpServer移除当前连接对象时调用 */
    void connectionDestoryed();

    /** 设置上下文 */
    void setContext(const std::any& context);

    /** 获取上下文 */
    std::any& context();

private:
    /** 连接状态 */
    enum class State {
        Disconnected, ///< 已断开连接
        Connecting,   ///< 正在建立连接
        Connected,    ///< 已建立连接
        Disconnecting ///< 正在断开连接
    };

    /** 处理连接上的可读事件 */
    void handleRead(TimePoint receive_time);

    /** 处理连接上的可写事件 */
    void handleWrite();

    /** 处理连接上的关闭事件 */
    void handleClose();

    /** 处理连接上的错误事件 */
    void handleError();

    /** 向对端发送消息 */
    void send_(const std::string& message);

    /** 关闭连接 */
    void shutdown_();

    /** 强制关闭连接 */
    void forceClose_();

    /** 开始读数据 */
    void startRead_();

    /** 停止读数据 */
    void stopRead_();

    /** 将连接状态转换为字符串形式 */
    static std::string toString(State state);

private:
    EventLoop*        loop_; ///< 事件循环
    const std::string name_; ///< 连接名称

    std::atomic<State> state_ { State::Connecting }; ///< 连接状态

    std::unique_ptr<Socket>  socket_;  ///< 连接套接字
    std::unique_ptr<Channel> channel_; ///< 连接套接字通道

    bool   reading_ { false }; ///< 是否正在连接上读取数据
    size_t high_water_mark_;   ///< 高水位标记

    const InetAddress local_addr_; ///< 本地地址
    const InetAddress peer_addr_;  ///< 对端地址

    ConnectionCallback    connection_cb_ {};      ///< 连接回调函数
    MessageCallback       message_cb_ {};         ///< 消息回调函数
    WriteCompleteCallback write_complete_cb_ {};  ///< 写操作完成回调函数
    HighWaterMarkCallback high_water_mark_cb_ {}; ///< 高水位标记回调函数
    CloseCallback         close_cb_ {};           ///< 关闭回调函数

    ByteBuffer input_buffer_;  ///< 输入缓冲区
    ByteBuffer output_buffer_; ///< 输出缓冲区

    std::any context_; ///< 上下文，用于携带额外数据
};
} // namespace talko::net