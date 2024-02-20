#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <net/callbacks.h>
#include <net/inet_address.h>

namespace talko::net {
class EventLoop;
class Acceptor;

/**
 * @brief TCP服务器
 *
 */
class TcpServer {
public:
    /**
     * @brief Construct a new Tcp Server object
     *
     * @param loop 事件循环
     * @param listen_addr 监听地址
     * @param name 服务器名称
     * @param reuse_port 是否复用端口
     */
    TcpServer(EventLoop* loop, const InetAddress& listen_addr, const std::string& name, bool reuse_port = false);
    ~TcpServer();

    TcpServer(const TcpServer&)            = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    TcpServer(TcpServer&&)            = delete;
    TcpServer& operator=(TcpServer&&) = delete;

    /** 获取字符串表示的IP地址和端口号 */
    const std::string& ipPort() const;

    /** 获取服务器名称 */
    const std::string& name() const;

    /** 获取事件循环 */
    EventLoop* loop() const;

    /** 启动服务器 */
    void start();

    /** 设置子事件循环的数目 */
    void setSubLoopSize(size_t size);

    /** 设置连接回调函数 */
    void setConnectionCallback(ConnectionCallback cb);

    /** 设置消息回调函数 */
    void setMessageCallback(MessageCallback cb);

    /** 设置写操作完成回调函数 */
    void setWriteCompleteCallback(WriteCompleteCallback cb);

private:
    /**
     * @brief 处理新的客户端连接
     *
     * @param sockfd 连接套接字
     * @param peer_addr 对端地址
     */
    void newConnection(int sockfd, const InetAddress& peer_addr);

    /** 移除客户端连接 */
    void removeConnection(const TcpConnectionPtr& conn);

    /** 移除客户端连接 */
    void removeConnection_(const TcpConnectionPtr& conn);

    /** 启动子事件循环 */
    void startSubEventLoop();

    /** 获取下一事件循环 */
    EventLoop* getNextLoop();

private:
    using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

    EventLoop*       main_loop_;         ///< 主事件循环
    std::atomic_bool started_ { false }; ///< 服务器是否已经启动

    const std::string ip_port_; ///< 字符串表示的IP地址和端口号
    const std::string name_;    ///< 服务器名称

    std::unique_ptr<Acceptor> acceptor_; ///< 接收器

    ConnectionCallback    connection_cb_ {};     ///< 连接回调函数
    MessageCallback       message_cb_ {};        ///< 消息回调函数
    WriteCompleteCallback write_complete_cb_ {}; ///< 写操作完成回调函数

    ConnectionMap connections_; ///< 连接映射表
    std::mutex    mtx_;         ///< 保护子事件循环列表的线程安全

    std::vector<EventLoop*> sub_loops_; ///< 子事件循环列表

    size_t next_ { 0 };          ///< 下一个子事件循环的编号
    size_t sub_loop_size_ { 3 }; ///< 子事件循环数目
    size_t nxt_conn_id_ { 1 };   ///< 连接编号
};
} // namespace talko::net