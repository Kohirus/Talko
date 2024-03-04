#pragma once

#include <google/protobuf/service.h>
#include <net/net.h>
#include <rpc/rpc_types.h>

namespace talko::rpc {
class RpcChannel : public google::protobuf::RpcChannel {
public:
    /**
     * @brief Construct a new Rpc Channel object
     *
     * @param discover_timeout 发现服务的超时时间
     */
    RpcChannel(net::Duration discover_timeout);
    ~RpcChannel() = default;

    RpcChannel(const RpcChannel&)            = delete;
    RpcChannel& operator=(const RpcChannel&) = delete;

private:
    /**
     * @brief 调用远程服务的给定方法
     *
     * @param method 服务方法
     * @param controller 服务控制器
     * @param request RPC请求
     * @param response RPC响应
     * @param done 回调函数
     */
    void CallMethod(MethodDescriptorPtr method, RpcControllerPtr controller,
        ConstMessagePtr request, MessagePtr response, ClosurePtr done) override;

    /** 组织请求内容 */
    void createRequestContent(const std::string& service_name, const std::string& method_name,
        RpcControllerPtr controller, ConstMessagePtr request);

    /** 连接回调函数 */
    void onConnection(const net::TcpConnectionPtr& conn);

    /** 消息回调函数 */
    void onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time);

    /** 连接超时 */
    void connectTimeout();

private:
    net::Duration discover_timeout_;     ///< 发现的超时时间
    net::Duration remaining_timeout_;    ///< 剩余的超时时间
    net::TimerId  response_timer_;       ///< 接收服务响应的定时器
    std::string   buffer_;               ///< 缓冲区
    bool          is_timeout_ { false }; ///< 是否超时

    net::TcpClient* client_ { nullptr }; ///< 客户端
    net::EventLoop* loop_ { nullptr };   ///< 事件循环
};
} // namespace talko::rpc