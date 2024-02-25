#include <rpc/rpc_application.h>
using namespace talko;

int main(int argc, char* argv[]) {
    rpc::RpcApplication::instance().init(argc, argv);

    return 0;
}