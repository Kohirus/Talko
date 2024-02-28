#include "user.pb.h"
#include <rpc/rpc_application.h>
using namespace talko;

int main(int argc, char* argv[]) {
    rpc::RpcApplication::instance().init(argc, argv);

    fixbug::UserServiceRpc_Stub callee(new rpc::RpcChannel(std::chrono::seconds(5)));

    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    fixbug::LoginResponse response;

    rpc::RpcController controller;

    callee.Login(&controller, &request, &response, nullptr);

    if (controller.failed()) {
        LOG_ERROR("Failed to execute RPC: {}", controller.errorMessage());
    } else {
        if (response.result().errcode() == 0) {
            LOG_INFO("RPC Login response: {}", response.success());
        } else {
            LOG_INFO("RPC Login response error: {}", response.result().errmsg());
        }
    }

    return 0;
}