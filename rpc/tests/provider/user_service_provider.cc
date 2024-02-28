#include "user.pb.h"
#include <rpc/rpc_application.h>
#include <rpc/rpc_provider.h>
using namespace talko;

class UserService : public fixbug::UserServiceRpc {
public:
    bool Login(std::string name, std::string pwd) {
        LOG_DEBUG("Handle local service: Login");
        LOG_INFO("Login: name[{}] pwd[{}]", name, pwd);
        return true;
    };

    bool Register(uint32_t id, std::string name, std::string pwd) {
        LOG_DEBUG("Handle local service: Register");
        LOG_INFO("Register: id[{}] name[{}] pwd[{}]", id, name, pwd);
        return true;
    }

    void Login(::google::protobuf::RpcController* controller,
        const ::fixbug::LoginRequest*             request,
        ::fixbug::LoginResponse*                  response,
        ::google::protobuf::Closure*              done) override {
        std::string name = request->name();
        std::string pwd  = request->pwd();

        bool login_request = Login(name, pwd); // 执行本地业务

        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_request);

        done->Run();
    }

    void Register(::google::protobuf::RpcController* controller,
        const ::fixbug::RegisterRequest*             request,
        ::fixbug::RegisterResponse*                  response,
        ::google::protobuf::Closure*                 done) override {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd  = request->pwd();

        bool ret = Register(id, name, pwd);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(ret);

        done->Run();
    }
};

int main(int argc, char* argv[]) {
    rpc::RpcApplication::instance().init(argc, argv);

    rpc::RpcProvider provider;
    provider.publish(new UserService());

    provider.run();

    return 0;
}