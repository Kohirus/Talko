#pragma once

#include <google/protobuf/service.h>
#include <rpc/rpc_types.h>

namespace talko::rpc {
class RpcController : public google::protobuf::RpcController {
public:
    RpcController()  = default;
    ~RpcController() = default;

    RpcController(const RpcController&)            = delete;
    RpcController& operator=(const RpcController&) = delete;

    RpcController(RpcController&&)            = delete;
    RpcController& operator=(RpcController&&) = delete;

    /** 重置控制器状态 */
    void reset();

    /** RPC调用是否失败 */
    bool failed() const;

    /** 返回RPC调用的错误信息 */
    std::string errorMessage() const;

    /** 取消RPC调用 */
    void cancel();

protected:
    void Reset() override;

    bool Failed() const override;

    std::string ErrorText() const override;

    void SetFailed(const std::string& reason) override;

    void StartCancel() override;

    bool IsCanceled() const override;

    void NotifyOnCancel(ClosurePtr cb) override;

private:
    bool        failed_ { false };   ///< RPC方法调用是否失败
    bool        canceled_ { false }; ///< RPC方法调用是否被取消
    std::string err_msg_ { "" };     ///< 错误消息
};
} // namespace talko::rpc