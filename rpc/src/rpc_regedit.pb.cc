// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rpc_regedit.proto

#include <rpc/rpc_regedit.pb.h>

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
extern PROTOBUF_INTERNAL_EXPORT_rpc_5fregedit_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_ServiceInstance_rpc_5fregedit_2eproto;
namespace talko {
namespace registry {
class ServiceInstanceDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<ServiceInstance> _instance;
} _ServiceInstance_default_instance_;
class ServiceRequestDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<ServiceRequest> _instance;
} _ServiceRequest_default_instance_;
class ServiceResponseDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<ServiceResponse> _instance;
} _ServiceResponse_default_instance_;
}  // namespace registry
}  // namespace talko
static void InitDefaultsscc_info_ServiceInstance_rpc_5fregedit_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::talko::registry::_ServiceInstance_default_instance_;
    new (ptr) ::talko::registry::ServiceInstance();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::talko::registry::ServiceInstance::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_ServiceInstance_rpc_5fregedit_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_ServiceInstance_rpc_5fregedit_2eproto}, {}};

static void InitDefaultsscc_info_ServiceRequest_rpc_5fregedit_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::talko::registry::_ServiceRequest_default_instance_;
    new (ptr) ::talko::registry::ServiceRequest();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::talko::registry::ServiceRequest::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_ServiceRequest_rpc_5fregedit_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_ServiceRequest_rpc_5fregedit_2eproto}, {
      &scc_info_ServiceInstance_rpc_5fregedit_2eproto.base,}};

static void InitDefaultsscc_info_ServiceResponse_rpc_5fregedit_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::talko::registry::_ServiceResponse_default_instance_;
    new (ptr) ::talko::registry::ServiceResponse();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::talko::registry::ServiceResponse::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_ServiceResponse_rpc_5fregedit_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_ServiceResponse_rpc_5fregedit_2eproto}, {
      &scc_info_ServiceInstance_rpc_5fregedit_2eproto.base,}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_rpc_5fregedit_2eproto[3];
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_rpc_5fregedit_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_rpc_5fregedit_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_rpc_5fregedit_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceInstance, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceInstance, service_name_),
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceInstance, method_name_),
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceInstance, address_),
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceInstance, port_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceRequest, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceRequest, msg_type_),
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceRequest, instance_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceResponse, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceResponse, msg_type_),
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceResponse, success_),
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceResponse, err_msg_),
  PROTOBUF_FIELD_OFFSET(::talko::registry::ServiceResponse, instance_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::talko::registry::ServiceInstance)},
  { 9, -1, sizeof(::talko::registry::ServiceRequest)},
  { 16, -1, sizeof(::talko::registry::ServiceResponse)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::talko::registry::_ServiceInstance_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::talko::registry::_ServiceRequest_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::talko::registry::_ServiceResponse_default_instance_),
};

const char descriptor_table_protodef_rpc_5fregedit_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\021rpc_regedit.proto\022\016talko.registry\"[\n\017S"
  "erviceInstance\022\024\n\014service_name\030\001 \001(\014\022\023\n\013"
  "method_name\030\002 \001(\014\022\017\n\007address\030\003 \001(\014\022\014\n\004po"
  "rt\030\004 \001(\005\"r\n\016ServiceRequest\022-\n\010msg_type\030\001"
  " \001(\0162\033.talko.registry.MessageType\0221\n\010ins"
  "tance\030\002 \001(\0132\037.talko.registry.ServiceInst"
  "ance\"\225\001\n\017ServiceResponse\022-\n\010msg_type\030\001 \001"
  "(\0162\033.talko.registry.MessageType\022\017\n\007succe"
  "ss\030\002 \001(\010\022\017\n\007err_msg\030\003 \001(\014\0221\n\010instance\030\004 "
  "\001(\0132\037.talko.registry.ServiceInstance*G\n\013"
  "MessageType\022\014\n\010REGISTER\020\000\022\014\n\010DISCOVER\020\001\022"
  "\r\n\tHEARTBEAT\020\002\022\r\n\tBROADCAST\020\003b\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_rpc_5fregedit_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_rpc_5fregedit_2eproto_sccs[3] = {
  &scc_info_ServiceInstance_rpc_5fregedit_2eproto.base,
  &scc_info_ServiceRequest_rpc_5fregedit_2eproto.base,
  &scc_info_ServiceResponse_rpc_5fregedit_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_rpc_5fregedit_2eproto_once;
static bool descriptor_table_rpc_5fregedit_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_rpc_5fregedit_2eproto = {
  &descriptor_table_rpc_5fregedit_2eproto_initialized, descriptor_table_protodef_rpc_5fregedit_2eproto, "rpc_regedit.proto", 477,
  &descriptor_table_rpc_5fregedit_2eproto_once, descriptor_table_rpc_5fregedit_2eproto_sccs, descriptor_table_rpc_5fregedit_2eproto_deps, 3, 0,
  schemas, file_default_instances, TableStruct_rpc_5fregedit_2eproto::offsets,
  file_level_metadata_rpc_5fregedit_2eproto, 3, file_level_enum_descriptors_rpc_5fregedit_2eproto, file_level_service_descriptors_rpc_5fregedit_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_rpc_5fregedit_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_rpc_5fregedit_2eproto), true);
namespace talko {
namespace registry {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* MessageType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_rpc_5fregedit_2eproto);
  return file_level_enum_descriptors_rpc_5fregedit_2eproto[0];
}
bool MessageType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}


// ===================================================================

void ServiceInstance::InitAsDefaultInstance() {
}
class ServiceInstance::_Internal {
 public:
};

ServiceInstance::ServiceInstance()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:talko.registry.ServiceInstance)
}
ServiceInstance::ServiceInstance(const ServiceInstance& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  service_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_service_name().empty()) {
    service_name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.service_name_);
  }
  method_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_method_name().empty()) {
    method_name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.method_name_);
  }
  address_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_address().empty()) {
    address_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.address_);
  }
  port_ = from.port_;
  // @@protoc_insertion_point(copy_constructor:talko.registry.ServiceInstance)
}

void ServiceInstance::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_ServiceInstance_rpc_5fregedit_2eproto.base);
  service_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  method_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  address_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  port_ = 0;
}

ServiceInstance::~ServiceInstance() {
  // @@protoc_insertion_point(destructor:talko.registry.ServiceInstance)
  SharedDtor();
}

void ServiceInstance::SharedDtor() {
  service_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  method_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  address_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void ServiceInstance::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const ServiceInstance& ServiceInstance::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_ServiceInstance_rpc_5fregedit_2eproto.base);
  return *internal_default_instance();
}


void ServiceInstance::Clear() {
// @@protoc_insertion_point(message_clear_start:talko.registry.ServiceInstance)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  service_name_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  method_name_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  address_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  port_ = 0;
  _internal_metadata_.Clear();
}

const char* ServiceInstance::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // bytes service_name = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_service_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // bytes method_name = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_method_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // bytes address = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_address();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // int32 port = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          port_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* ServiceInstance::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:talko.registry.ServiceInstance)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // bytes service_name = 1;
  if (this->service_name().size() > 0) {
    target = stream->WriteBytesMaybeAliased(
        1, this->_internal_service_name(), target);
  }

  // bytes method_name = 2;
  if (this->method_name().size() > 0) {
    target = stream->WriteBytesMaybeAliased(
        2, this->_internal_method_name(), target);
  }

  // bytes address = 3;
  if (this->address().size() > 0) {
    target = stream->WriteBytesMaybeAliased(
        3, this->_internal_address(), target);
  }

  // int32 port = 4;
  if (this->port() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(4, this->_internal_port(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:talko.registry.ServiceInstance)
  return target;
}

size_t ServiceInstance::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:talko.registry.ServiceInstance)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // bytes service_name = 1;
  if (this->service_name().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_service_name());
  }

  // bytes method_name = 2;
  if (this->method_name().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_method_name());
  }

  // bytes address = 3;
  if (this->address().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_address());
  }

  // int32 port = 4;
  if (this->port() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_port());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void ServiceInstance::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:talko.registry.ServiceInstance)
  GOOGLE_DCHECK_NE(&from, this);
  const ServiceInstance* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<ServiceInstance>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:talko.registry.ServiceInstance)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:talko.registry.ServiceInstance)
    MergeFrom(*source);
  }
}

void ServiceInstance::MergeFrom(const ServiceInstance& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:talko.registry.ServiceInstance)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.service_name().size() > 0) {

    service_name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.service_name_);
  }
  if (from.method_name().size() > 0) {

    method_name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.method_name_);
  }
  if (from.address().size() > 0) {

    address_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.address_);
  }
  if (from.port() != 0) {
    _internal_set_port(from._internal_port());
  }
}

void ServiceInstance::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:talko.registry.ServiceInstance)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ServiceInstance::CopyFrom(const ServiceInstance& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:talko.registry.ServiceInstance)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ServiceInstance::IsInitialized() const {
  return true;
}

void ServiceInstance::InternalSwap(ServiceInstance* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  service_name_.Swap(&other->service_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  method_name_.Swap(&other->method_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  address_.Swap(&other->address_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(port_, other->port_);
}

::PROTOBUF_NAMESPACE_ID::Metadata ServiceInstance::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void ServiceRequest::InitAsDefaultInstance() {
  ::talko::registry::_ServiceRequest_default_instance_._instance.get_mutable()->instance_ = const_cast< ::talko::registry::ServiceInstance*>(
      ::talko::registry::ServiceInstance::internal_default_instance());
}
class ServiceRequest::_Internal {
 public:
  static const ::talko::registry::ServiceInstance& instance(const ServiceRequest* msg);
};

const ::talko::registry::ServiceInstance&
ServiceRequest::_Internal::instance(const ServiceRequest* msg) {
  return *msg->instance_;
}
ServiceRequest::ServiceRequest()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:talko.registry.ServiceRequest)
}
ServiceRequest::ServiceRequest(const ServiceRequest& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  if (from._internal_has_instance()) {
    instance_ = new ::talko::registry::ServiceInstance(*from.instance_);
  } else {
    instance_ = nullptr;
  }
  msg_type_ = from.msg_type_;
  // @@protoc_insertion_point(copy_constructor:talko.registry.ServiceRequest)
}

void ServiceRequest::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_ServiceRequest_rpc_5fregedit_2eproto.base);
  ::memset(&instance_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&msg_type_) -
      reinterpret_cast<char*>(&instance_)) + sizeof(msg_type_));
}

ServiceRequest::~ServiceRequest() {
  // @@protoc_insertion_point(destructor:talko.registry.ServiceRequest)
  SharedDtor();
}

void ServiceRequest::SharedDtor() {
  if (this != internal_default_instance()) delete instance_;
}

void ServiceRequest::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const ServiceRequest& ServiceRequest::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_ServiceRequest_rpc_5fregedit_2eproto.base);
  return *internal_default_instance();
}


void ServiceRequest::Clear() {
// @@protoc_insertion_point(message_clear_start:talko.registry.ServiceRequest)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  if (GetArenaNoVirtual() == nullptr && instance_ != nullptr) {
    delete instance_;
  }
  instance_ = nullptr;
  msg_type_ = 0;
  _internal_metadata_.Clear();
}

const char* ServiceRequest::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // .talko.registry.MessageType msg_type = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_msg_type(static_cast<::talko::registry::MessageType>(val));
        } else goto handle_unusual;
        continue;
      // .talko.registry.ServiceInstance instance = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          ptr = ctx->ParseMessage(_internal_mutable_instance(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* ServiceRequest::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:talko.registry.ServiceRequest)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .talko.registry.MessageType msg_type = 1;
  if (this->msg_type() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      1, this->_internal_msg_type(), target);
  }

  // .talko.registry.ServiceInstance instance = 2;
  if (this->has_instance()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        2, _Internal::instance(this), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:talko.registry.ServiceRequest)
  return target;
}

size_t ServiceRequest::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:talko.registry.ServiceRequest)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // .talko.registry.ServiceInstance instance = 2;
  if (this->has_instance()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *instance_);
  }

  // .talko.registry.MessageType msg_type = 1;
  if (this->msg_type() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_msg_type());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void ServiceRequest::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:talko.registry.ServiceRequest)
  GOOGLE_DCHECK_NE(&from, this);
  const ServiceRequest* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<ServiceRequest>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:talko.registry.ServiceRequest)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:talko.registry.ServiceRequest)
    MergeFrom(*source);
  }
}

void ServiceRequest::MergeFrom(const ServiceRequest& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:talko.registry.ServiceRequest)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.has_instance()) {
    _internal_mutable_instance()->::talko::registry::ServiceInstance::MergeFrom(from._internal_instance());
  }
  if (from.msg_type() != 0) {
    _internal_set_msg_type(from._internal_msg_type());
  }
}

void ServiceRequest::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:talko.registry.ServiceRequest)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ServiceRequest::CopyFrom(const ServiceRequest& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:talko.registry.ServiceRequest)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ServiceRequest::IsInitialized() const {
  return true;
}

void ServiceRequest::InternalSwap(ServiceRequest* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(instance_, other->instance_);
  swap(msg_type_, other->msg_type_);
}

::PROTOBUF_NAMESPACE_ID::Metadata ServiceRequest::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void ServiceResponse::InitAsDefaultInstance() {
  ::talko::registry::_ServiceResponse_default_instance_._instance.get_mutable()->instance_ = const_cast< ::talko::registry::ServiceInstance*>(
      ::talko::registry::ServiceInstance::internal_default_instance());
}
class ServiceResponse::_Internal {
 public:
  static const ::talko::registry::ServiceInstance& instance(const ServiceResponse* msg);
};

const ::talko::registry::ServiceInstance&
ServiceResponse::_Internal::instance(const ServiceResponse* msg) {
  return *msg->instance_;
}
ServiceResponse::ServiceResponse()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:talko.registry.ServiceResponse)
}
ServiceResponse::ServiceResponse(const ServiceResponse& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  err_msg_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_err_msg().empty()) {
    err_msg_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.err_msg_);
  }
  if (from._internal_has_instance()) {
    instance_ = new ::talko::registry::ServiceInstance(*from.instance_);
  } else {
    instance_ = nullptr;
  }
  ::memcpy(&msg_type_, &from.msg_type_,
    static_cast<size_t>(reinterpret_cast<char*>(&success_) -
    reinterpret_cast<char*>(&msg_type_)) + sizeof(success_));
  // @@protoc_insertion_point(copy_constructor:talko.registry.ServiceResponse)
}

void ServiceResponse::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_ServiceResponse_rpc_5fregedit_2eproto.base);
  err_msg_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&instance_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&success_) -
      reinterpret_cast<char*>(&instance_)) + sizeof(success_));
}

ServiceResponse::~ServiceResponse() {
  // @@protoc_insertion_point(destructor:talko.registry.ServiceResponse)
  SharedDtor();
}

void ServiceResponse::SharedDtor() {
  err_msg_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete instance_;
}

void ServiceResponse::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const ServiceResponse& ServiceResponse::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_ServiceResponse_rpc_5fregedit_2eproto.base);
  return *internal_default_instance();
}


void ServiceResponse::Clear() {
// @@protoc_insertion_point(message_clear_start:talko.registry.ServiceResponse)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  err_msg_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (GetArenaNoVirtual() == nullptr && instance_ != nullptr) {
    delete instance_;
  }
  instance_ = nullptr;
  ::memset(&msg_type_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&success_) -
      reinterpret_cast<char*>(&msg_type_)) + sizeof(success_));
  _internal_metadata_.Clear();
}

const char* ServiceResponse::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // .talko.registry.MessageType msg_type = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_msg_type(static_cast<::talko::registry::MessageType>(val));
        } else goto handle_unusual;
        continue;
      // bool success = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          success_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // bytes err_msg = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_err_msg();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // .talko.registry.ServiceInstance instance = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 34)) {
          ptr = ctx->ParseMessage(_internal_mutable_instance(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* ServiceResponse::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:talko.registry.ServiceResponse)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .talko.registry.MessageType msg_type = 1;
  if (this->msg_type() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      1, this->_internal_msg_type(), target);
  }

  // bool success = 2;
  if (this->success() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(2, this->_internal_success(), target);
  }

  // bytes err_msg = 3;
  if (this->err_msg().size() > 0) {
    target = stream->WriteBytesMaybeAliased(
        3, this->_internal_err_msg(), target);
  }

  // .talko.registry.ServiceInstance instance = 4;
  if (this->has_instance()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        4, _Internal::instance(this), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:talko.registry.ServiceResponse)
  return target;
}

size_t ServiceResponse::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:talko.registry.ServiceResponse)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // bytes err_msg = 3;
  if (this->err_msg().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_err_msg());
  }

  // .talko.registry.ServiceInstance instance = 4;
  if (this->has_instance()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *instance_);
  }

  // .talko.registry.MessageType msg_type = 1;
  if (this->msg_type() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_msg_type());
  }

  // bool success = 2;
  if (this->success() != 0) {
    total_size += 1 + 1;
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void ServiceResponse::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:talko.registry.ServiceResponse)
  GOOGLE_DCHECK_NE(&from, this);
  const ServiceResponse* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<ServiceResponse>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:talko.registry.ServiceResponse)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:talko.registry.ServiceResponse)
    MergeFrom(*source);
  }
}

void ServiceResponse::MergeFrom(const ServiceResponse& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:talko.registry.ServiceResponse)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.err_msg().size() > 0) {

    err_msg_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.err_msg_);
  }
  if (from.has_instance()) {
    _internal_mutable_instance()->::talko::registry::ServiceInstance::MergeFrom(from._internal_instance());
  }
  if (from.msg_type() != 0) {
    _internal_set_msg_type(from._internal_msg_type());
  }
  if (from.success() != 0) {
    _internal_set_success(from._internal_success());
  }
}

void ServiceResponse::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:talko.registry.ServiceResponse)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ServiceResponse::CopyFrom(const ServiceResponse& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:talko.registry.ServiceResponse)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ServiceResponse::IsInitialized() const {
  return true;
}

void ServiceResponse::InternalSwap(ServiceResponse* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  err_msg_.Swap(&other->err_msg_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(instance_, other->instance_);
  swap(msg_type_, other->msg_type_);
  swap(success_, other->success_);
}

::PROTOBUF_NAMESPACE_ID::Metadata ServiceResponse::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace registry
}  // namespace talko
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::talko::registry::ServiceInstance* Arena::CreateMaybeMessage< ::talko::registry::ServiceInstance >(Arena* arena) {
  return Arena::CreateInternal< ::talko::registry::ServiceInstance >(arena);
}
template<> PROTOBUF_NOINLINE ::talko::registry::ServiceRequest* Arena::CreateMaybeMessage< ::talko::registry::ServiceRequest >(Arena* arena) {
  return Arena::CreateInternal< ::talko::registry::ServiceRequest >(arena);
}
template<> PROTOBUF_NOINLINE ::talko::registry::ServiceResponse* Arena::CreateMaybeMessage< ::talko::registry::ServiceResponse >(Arena* arena) {
  return Arena::CreateInternal< ::talko::registry::ServiceResponse >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
