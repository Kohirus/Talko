#pragma once

#include <memory>
#include <net/net.h>
#include <registry/service_manager.h>

namespace talko::registry {
class ServiceInstance;

/**
 * @brief 注册中心
 *
 */
class Registry {
public:
    /**
     * @brief Construct a new Registry object
     *
     * @param loop 事件循环
     */
    Registry(net::EventLoop* loop);
    ~Registry() = default;

    Registry(const Registry&)            = delete;
    Registry& operator=(const Registry&) = delete;

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
     * @param conn 连接对象
     */
    void registerMethod(const std::string& service_name, const std::string& method_name, const net::TcpConnectionPtr& conn);

    /**
     * @brief 发现方法
     *
     * @param service_name 服务名称
     * @param method_name 方法名称
     * @param conn 连接对象
     */
    void discoverMethod(const std::string& service_name, const std::string& method_name, const net::TcpConnectionPtr& conn);

    /** 请求成功 */
    void requestSuccess(const net::TcpConnectionPtr& conn, ServiceInstance* instance);

    /** 请求失败 */
    void requestError(const net::TcpConnectionPtr& conn, const std::string& err_msg);

private:
    using ServiceManagerPtr = std::unique_ptr<ServiceManager>;

    net::TcpServer    server_;  ///< 服务器
    ServiceManagerPtr manager_; ///< 服务管理者
};
} // namespace talko::registry