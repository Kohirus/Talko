#pragma once

#include <google/protobuf/service.h>
#include <net/net.h>
#include <rpc/rpc_types.h>

namespace talko::rpc {
/**
 * @brief 异步RPC通道
 * 
 */
class RpcAsyncChannel : public google::protobuf::RpcChannel {
public:
    /**
     * @brief Construct a new Rpc Channel object
     *
     * @param timeout RPC请求的超时时间
     */
    RpcAsyncChannel(net::Duration timeout);
    ~RpcAsyncChannel() = default;

    RpcAsyncChannel(const RpcAsyncChannel&)            = delete;
    RpcAsyncChannel& operator=(const RpcAsyncChannel&) = delete;

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

private:
    /** 连接回调函数 */
    void onConnection(const net::TcpConnectionPtr& conn);

    /** 消息回调函数 */
    void onMessage(const net::TcpConnectionPtr& conn, net::ByteBuffer* buffer, net::TimePoint time);

    /** 处理RPC请求的超时响应 */
    void handleTimeout(RpcControllerPtr controller);

    /** 在子线程中处理RPC请求 */
    void handleRequest(const std::string& service_name, const std::string& method_name,
        RpcControllerPtr controller, ConstMessagePtr request, MessagePtr response, ClosurePtr done);

private:
    std::unique_ptr<net::TcpClient> client_ { nullptr };

    std::string package_; ///< 待发送的RPC内容
    std::string result_;  ///< 接收的远程RPC调用结果

    net::Duration timeout_; ///< 超时时间
};
} // namespace talko::rpc