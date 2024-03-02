#pragma once

#include <google/protobuf/service.h>
#include <net/net.h>
#include <rpc/rpc_types.h>

namespace talko::rpc {
class RpcChannel : public google::protobuf::RpcChannel {
public:
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
    std::string createRequestContent(const std::string& service_name, const std::string& method_name,
        RpcControllerPtr controller, ConstMessagePtr request);

private:
    net::Duration discover_timeout_; ///< 发现的超时时间
};
} // namespace talko::rpc