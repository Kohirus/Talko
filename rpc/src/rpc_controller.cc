#include <rpc/rpc_controller.h>

namespace talko::rpc {
void RpcController::reset() {
    Reset();
}

bool RpcController::failed() const {
    return Failed();
}

std::string RpcController::errorMessage() const {
    return ErrorText();
}

void RpcController::cancel() {
    StartCancel();
}

void RpcController::Reset() {
    failed_ = false;
    err_msg_.clear();
}

bool RpcController::Failed() const {
    return failed_;
}

std::string RpcController::ErrorText() const {
    return err_msg_;
}

void RpcController::SetFailed(const std::string& reason) {
    failed_  = true;
    err_msg_ = reason;
}

void RpcController::StartCancel() {
    canceled_ = true;
}

bool RpcController::IsCanceled() const {
    return canceled_;
}

void RpcController::NotifyOnCancel(ClosurePtr cb) {
    cb->Run();
}
} // namespace talko::rpc