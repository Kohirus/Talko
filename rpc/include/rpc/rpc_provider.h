#pragma once

#include <google/protobuf/service.h>
#include <net/net.h>

namespace talko::rpc {
/**
 * @brief RPC服务提供方
 *
 */
class RpcProvider {
public:
    /** 发布RPC服务 */
    void notifyService(google::protobuf::Service* service);

    /** 启动RPC服务节点 */
    void run();

private:
    /** 连接回调函数 */
    void onConnection(const net::TcpConnectionPtr& conn);

    /** 消息回调函数 */
    void onMessage(const net::TcpConnectionPtr& conn,
        net::ByteBuffer* buffer, net::TimePoint time);

private:
    net::EventLoop loop_; ///< 事件循环
};
} // namespace talko::rpc