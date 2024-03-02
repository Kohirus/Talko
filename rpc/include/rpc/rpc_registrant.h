#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <net/net.h>

namespace talko::rpc {
/**
 * @brief RPC注册中心客户端
 *
 */
class RpcRegistrant {
public:
    /** 获取实例对象 */
    static RpcRegistrant& instance();

    /**
     * @brief 阻塞当前线程以连接注册中心
     *
     * @param connect_timeout 连接注册中心的超时时间
     * @param heartbeat_interval 心跳包的发送时间间隔
     * @param server_addr 注册中心服务器地址
     * @return 连接成功则返回true，否则返回false
     */
    bool connect(net::Duration connect_timeout, net::Duration heartbeat_interval, const net::InetAddress& server_addr);

    /** 是否已与注册中心建立连接 */
    inline bool connected() const { return connected_; }

    /**
     * @brief 阻塞以注册方法
     *
     * @param service_name 服务名称
     * @param method_name 方法名称
     * @param timeout 超时时间
     * @return 注册成功则返回true，否则返回false
     */
    bool enrollMethod(const std::string& service_name, const std::string& method_name, net::Duration timeout);

    /**
     * @brief 阻塞以发现方法
     *
     * @param[in] service_name 服务名称
     * @param[in] method_name 方法名称
     * @param[in] timeout 超时时间
     * @param[out] provider_addr 服务提供者的网络地址
     * @return 发现成功则返回true，否则返回false
     */
    bool discoverMethod(const std::string& service_name, const std::string& method_name, net::Duration timeout, net::InetAddress& provider_addr);

    /** 获取错误消息 */
    std::string errorMessage();

private:
    RpcRegistrant();
    ~RpcRegistrant();

    /** 重置错误消息 */
    void resetErrorMessage();

    /** 设置错误消息 */
    void setErrorMessage(const std::string& err_msg);

    /** 在子线程中连接注册中心 */
    void connect_();

    /** 连接回调函数 */
    void onConnection(const net::TcpConnectionPtr& conn);

    /** 消息回调函数 */
    void onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time);

    /** 连接超时 */
    void connectTimeout();

    /** 发送心跳数据，使注册中心确定当前节点能正常工作 */
    void heartbeat(const net::TcpConnectionPtr& conn);

    /** 在子线程中向注册中心注册方法 */
    void enrollMethod_(const std::string& service_name, const std::string& method_name);

    /** 在子线程中向注册中心请求方法 */
    void discoverMethod_(const std::string& service_name, const std::string& method_name);

private:
    net::EventLoop* loop_ { nullptr }; ///< 事件循环
    std::mutex      mtx_;              ///< 互斥锁
    std::string     err_msg_;          ///< 错误消息

    net::TimerId connect_timer_;   ///< 连接超时定时器
    net::TimerId heartbeat_timer_; ///< 心跳包定时器

    std::atomic_bool connected_ { false }; ///< 是否建立连接

    bool response_finished_ { false }; ///< 是否响应完成
    bool response_success_ { false };  ///< 是否响应成功

    net::Duration connect_timeout_;    ///< 连接超时时间
    net::Duration heartbeat_interval_; ///< 心跳包的间隔

    std::condition_variable cond_;               ///< 条件变量
    net::TcpConnectionPtr   conn_ { nullptr };   ///< 记录与注册中心的连接
    net::TcpClient*         client_ { nullptr }; ///< 记录客户端

    std::future<decltype(void())> task_ret_; ///< 子线程任务返回值

    net::InetAddress service_addr_;  ///< 服务提供者的地址
    net::InetAddress registry_addr_; ///< 注册中心的地址
};
} // namespace talko::rpc