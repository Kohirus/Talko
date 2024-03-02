#include <registry/registry_center.h>
#include <rpc/rpc_application.h>
using namespace talko;

int main(int argc, char* argv[]) {
    rpc::RpcApplication::instance().init(argc, argv, true);

    net::EventLoop           loop;
    registry::RegistryCenter reg(&loop);

    reg.start();
    loop.loop();

    return 0;
}