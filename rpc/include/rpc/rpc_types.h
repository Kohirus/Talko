#pragma once

namespace google::protobuf {
class Service;
class Message;
class Closure;
class RpcController;
class ServiceDescriptor;
class MethodDescriptor;
} // namespace google::protobuf

namespace talko::rpc {
using ServicePtr           = google::protobuf::Service*;
using MessagePtr           = google::protobuf::Message*;
using ConstMessagePtr      = const google::protobuf::Message*;
using ClosurePtr           = google::protobuf::Closure*;
using RpcControllerPtr     = google::protobuf::RpcController*;
using ServiceDescriptorPtr = const google::protobuf::ServiceDescriptor*;
using MethodDescriptorPtr  = const google::protobuf::MethodDescriptor*;
} // namespace talko::rpc