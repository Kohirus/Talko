#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <rpc/rpc_application.h>
#include <rpc/rpc_channel.h>
#include <rpc/rpc_header.pb.h>
#include <rpc/rpc_regedit.pb.h>

namespace talko::rpc {
RpcChannel::RpcChannel(net::Duration discover_timeout)
    : discover_timeout_(discover_timeout) {
}

void RpcChannel::CallMethod(MethodDescriptorPtr method, RpcControllerPtr controller,
    ConstMessagePtr request, MessagePtr response, ClosurePtr done) {
    LOGGER_TRACE("rpc", "Call method to get response of service");
    ServiceDescriptorPtr service = method->service();

    // 获取服务名称和方法名称
    std::string service_name = service->name();
    std::string method_name  = method->name();

    net::InetAddress service_addr;
    if (!RpcRegistrant::instance().discoverMethod(service_name, method_name, discover_timeout_, service_addr)) {
        controller->SetFailed(RpcRegistrant::instance().errorMessage());
        if (done) done->Run();
        return;
    }

    LOGGER_INFO("rpc", "[{}]-[{}] is located on {}", service_name, method_name, service_addr.toIpPort());
}

std::string RpcChannel::createRequestContent(const std::string& service_name, const std::string& method_name, RpcControllerPtr controller, ConstMessagePtr request) {
    // 序列化RPC请求参数
    std::string args_content;
    if (!request->SerializeToString(&args_content)) {
        controller->SetFailed("Failed to serialize request");
        return "";
    } else {
        LOGGER_TRACE("rpc", "Request serialization is complete");
    }

    // 获取RPC请求参数的大小
    size_t args_size = args_content.size();

    // 生成RPC请求头
    RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(static_cast<uint32_t>(args_size));

    // 序列化RPC请求头
    std::string header_content;
    if (!rpc_header.SerializeToString(&header_content)) {
        controller->SetFailed("Failed to serialize RpcHeader");
        return "";
    } else {
        LOGGER_TRACE("rpc", "RpcHeader serialization is complete");
    }

    // 获取RPC请求头的大小
    uint32_t header_size = static_cast<uint32_t>(header_content.size());

    // 组织待发送的RPC内容
    // -----------------------------------------------------------------------------------------------
    // | Header Content Size(4) | Header Content(Service Name,Method Name,Args Size) | Request Args |
    // -----------------------------------------------------------------------------------------------
    std::string content;
    content.append(std::string(reinterpret_cast<const char*>(&header_size), 4));
    content.append(header_content);
    content.append(args_content);

    LOGGER_TRACE("rpc", "Packaged RpcHeader: HeaderSize[{}] HeaderContent[{}] ServiceName[{}] MethodName[{}] ArgsSize[{}] Args[{}]",
        header_size, header_content, service_name, method_name, args_size, args_content);

    return content;
}
} // namespace talko::rpc