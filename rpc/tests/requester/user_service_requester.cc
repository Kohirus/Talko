#include "user.pb.h"
#include <rpc/rpc_application.h>
#include <rpc/rpc_channel.h>
using namespace talko;

class ResultClosure : public google::protobuf::Closure {
public:
    ResultClosure(rpc::RpcController* controller, fixbug::LoginResponse* response)
        : controller_(controller)
        , response_(response) {
    }

    void Run() override {
        LOG_DEBUG("RPC Request is finished");
        if (controller_->failed()) {
            LOG_ERROR("Failed to execute RPC: {}", controller_->errorMessage());
        } else {
            if (response_->result().errcode() == 0) {
                LOG_INFO("RPC Login response: {}", response_->success());
            } else {
                LOG_INFO("RPC Login response error: {}", response_->result().errmsg());
            }
        }
    }

private:
    rpc::RpcController*    controller_;
    fixbug::LoginResponse* response_;
};

int main(int argc, char* argv[]) {
    rpc::RpcApplication::instance().init(argc, argv);

    {
        fixbug::UserServiceRpc_Stub callee(new rpc::RpcChannel(std::chrono::seconds(2)));

        fixbug::LoginRequest request;
        request.set_name("zhang san");
        request.set_pwd("123456");

        fixbug::LoginResponse response;

        rpc::RpcController controller;

        ResultClosure closure(&controller, &response);

        callee.Login(&controller, &request, &response, &closure);
    }

    {
        fixbug::UserServiceRpc_Stub callee(new rpc::RpcChannel(std::chrono::seconds(2)));

        fixbug::LoginRequest request;
        request.set_name("zhang san");
        request.set_pwd("123456");

        fixbug::LoginResponse response;

        rpc::RpcController controller;

        ResultClosure closure(&controller, &response);

        callee.Login(&controller, &request, &response, &closure);
    }

    return 0;
}