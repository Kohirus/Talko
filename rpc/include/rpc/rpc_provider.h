#pragma once

#include <net/net.h>
#include <rpc/rpc_registry.h>
#include <rpc/rpc_types.h>
#include <unordered_map>

namespace talko::rpc {

/**
 * @brief RPC服务提供方
 *
 */
class RpcProvider {
public:
    RpcProvider();
    ~RpcProvider() = default;

    RpcProvider(const RpcProvider&)            = delete;
    RpcProvider& operator=(const RpcProvider&) = delete;

    RpcProvider(RpcProvider&&)            = delete;
    RpcProvider& operator=(RpcProvider&&) = delete;

    /** 发布RPC服务 */
    void publish(ServicePtr service);

    /** 启动RPC服务节点 */
    void run();

private:
    /** 连接回调函数 */
    void onConnection(const net::TcpConnectionPtr& conn);

    /** 消息回调函数 */
    void onMessage(const net::TcpConnectionPtr& conn,
        net::ByteBuffer* buffer, net::TimePoint time);

    /** 序列化RPC响应数据并发送 */
    void sendRpcResponse(const net::TcpConnectionPtr& conn, MessagePtr response);

private:
    using MethodHash = std::unordered_map<std::string, MethodDescriptorPtr>;

    /**
     * @brief 服务信息
     *
     */
    struct ServiceInfo {
        ServiceInfo() = default;

        ServiceInfo(ServicePtr serivce_in, MethodHash methods_in)
            : service(serivce_in)
            , methods(methods_in) { }

        ServicePtr service { nullptr }; // 服务对象
        MethodHash methods;             // 服务方法
    };

    using ServiceHash = std::unordered_map<std::string, ServiceInfo>;

private:
    net::EventLoop loop_;     ///< 事件循环
    ServiceHash    services_; ///< 服务信息映射表
    RpcRegistry    register_; ///< 注册器
};
} // namespace talko::rpc