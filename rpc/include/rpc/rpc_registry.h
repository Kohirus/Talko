#pragma once

#include <condition_variable>
#include <mutex>
#include <net/net.h>

namespace talko::rpc {
/**
 * @brief RPC注册器
 *
 */
class RpcRegistry {
public:
    RpcRegistry(net::Duration timeout);
    ~RpcRegistry();

    /** 连接注册器 */
    bool connect();

    /**
     * @brief 注册方法
     *
     * @param service_name 服务名称
     * @param method_name 方法名称
     * @return 注册成功则返回true，否则返回false
     */
    bool registerMethod(const std::string& service_name, const std::string& method_name);

    /**
     * @brief 发现方法
     *
     * @param service_name 服务名称
     * @param method_name 方法名称
     * @param provider_addr 服务提供者的网络地址
     * @return 发现成功则返回true，否则返回false
     */
    bool discoverMethod(const std::string& service_name, const std::string& method_name, net::InetAddress& provider_addr);

    /** 获取错误消息 */
    const std::string& errorMessage() const { return err_msg_; }

private:
    void connect_();

    /** 连接回调函数 */
    void onConnection(const net::TcpConnectionPtr& conn);

    /** 消息回调函数 */
    void onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time);

    void connectTimeout();

private:
    net::EventLoop* loop_ { nullptr };    ///< 事件循环
    std::string     err_msg_;             ///< 错误消息
    bool            connected_ { false }; ///< 是否建立连接
    net::Duration   timeout_;             ///< 超时时间
    net::TimerId    timer_id_;            ///< 定时器序号

    std::mutex              mtx_;  ///< 互斥锁
    std::condition_variable cond_; ///< 条件变量
};
} // namespace talko::rpc