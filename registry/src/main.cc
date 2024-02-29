#include <registry/registry.h>
#include <rpc/rpc_application.h>
using namespace talko;

int main(int argc, char* argv[]) {
    rpc::RpcApplication::instance().init(argc, argv);

    net::EventLoop     loop;
    registry::Registry reg(&loop);

    reg.start();
    loop.loop();

    return 0;
}