#pragma once

#include <memory>
#include <mutex>
#include <net/callbacks.h>
#include <net/inet_address.h>
#include <string>

namespace talko::net {
class EventLoop;
class Connector;

using ConnectorPtr = std::shared_ptr<Connector>;

/**
 * @brief TCP客户端
 *
 */
class TcpClient {
public:
    /**
     * @brief Construct a new Tcp Client object
     *
     * @param loop 事件循环
     * @param server_addr 服务器地址
     * @param name 客户端名称
     */
    TcpClient(EventLoop* loop, const InetAddress& server_addr, const std::string& name);
    ~TcpClient();

    TcpClient(const TcpClient&)            = delete;
    TcpClient& operator=(const TcpClient&) = delete;

    TcpClient(TcpClient&&)            = delete;
    TcpClient& operator=(TcpClient&&) = delete;

    /** 连接服务器 */
    void connect();

    /** 与服务器断开连接，关闭写端，但继续接收服务器发送的数据 */
    void disconnect();

    /** 停止客户端 */
    void stop();

    /** 获取与服务器的连接 */
    TcpConnectionPtr connection() const;

    /** 获取事件循环 */
    EventLoop* loop() const;

    /** 是否可以重连服务器 */
    bool retry() const;

    /** 开启服务器重连机制 */
    void enableRetry();

    /** 获取客户端名称 */
    const std::string& name() const;

    /** 设置连接回调函数 */
    void setConnectionCallback(ConnectionCallback cb);

    /** 设置消息回调函数 */
    void setMessageCallback(MessageCallback cb);

    /** 设置写完成回调函数 */
    void setWriteCompleteCallback(WriteCompleteCallback cb);

private:
    /** 处理与服务器的连接 */
    void newConnection(int sockfd);

    /** 断开与服务器的连接 */
    void removeConnection(const TcpConnectionPtr& conn);

private:
    EventLoop*        loop_;      ///< 事件循环
    ConnectorPtr      connector_; ///< 连接器
    const std::string name_;      ///< 客户端名称

    ConnectionCallback    connection_cb_;     ///< 连接回调
    MessageCallback       message_cb_;        ///< 消息回调
    WriteCompleteCallback write_complete_cb_; ///< 写完成回调

    TcpConnectionPtr connection_; ///< TCP连接管理器

    bool retry_ { false };   ///< 连接断开后是否重连
    bool connect_ { false }; ///< 是否已连接
    int  nxt_conn_id_ { 1 }; ///< 下一个连接编号

    std::mutex mtx_; ///< 保护TCP连接的线程安全
};
} // namespace talko::net