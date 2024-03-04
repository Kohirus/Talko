#pragma once

#include <memory>
#include <net/net.h>
#include <registry/rpc_regedit.pb.h>
#include <registry/service_manager.h>

namespace talko::registry {
/**
 * @brief 注册中心
 *
 */
class RegistryCenter {
public:
    /**
     * @brief Construct a new RegistryCenter object
     *
     * @param loop 事件循环
     */
    RegistryCenter(net::EventLoop* loop);
    ~RegistryCenter() = default;

    RegistryCenter(const RegistryCenter&)            = delete;
    RegistryCenter& operator=(const RegistryCenter&) = delete;

    /** 启动注册中心服务 */
    void start();

private:
    /** 连接回调函数 */
    void onConnection(const net::TcpConnectionPtr& conn);

    /** 消息回调函数 */
    void onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time);

    /**
     * @brief 注册方法
     *
     * @param service_name 服务名称
     * @param method_name 方法名称
     * @param provider_addr 服务提供者所在的网络地址
     * @param conn 连接对象
     */
    void enrollMethod(const std::string& service_name, const std::string& method_name, const net::InetAddress& proriver_addr, const net::TcpConnectionPtr& conn);

    /**
     * @brief 发现方法
     *
     * @param service_name 服务名称
     * @param method_name 方法名称
     * @param conn 连接对象
     */
    void discoverMethod(const std::string& service_name, const std::string& method_name, const net::TcpConnectionPtr& conn);

    /** 请求成功 */
    void requestSuccess(MessageType type, const net::TcpConnectionPtr& conn, ServiceInstance* instance);

    /** 请求失败 */
    void requestError(MessageType type, const net::TcpConnectionPtr& conn, const std::string& err_msg);

    /** 连接存活 */
    void connectionAlive(const net::TcpConnectionPtr& conn);

    /** 处理心跳超时 */
    void handleHeartbeatTimeout();

    /** 广播消息 */
    void broadcast(const std::string& service_name);

private:
    using ConnectionInfo    = std::pair<std::string, bool>;
    using ServiceManagerPtr = std::unique_ptr<ServiceManager>;
    using ConnectionMap     = std::unordered_map<net::TcpConnectionPtr, ConnectionInfo>;

    net::TcpServer    server_;  ///< 服务器
    ServiceManagerPtr manager_; ///< 服务管理者
    ConnectionMap     conns_;   ///< 管理所有的连接

    net::Duration heartbeat_timeout_; ///< 心跳检测的超时时间
};
} // namespace talko::registry