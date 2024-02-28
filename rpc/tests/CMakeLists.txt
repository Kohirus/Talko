add_executable(app_test app_test.cc)
target_link_libraries(app_test rpc)

# set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
# file(GLOB PROTO_FILES ${PROTO_DIR}/*.proto)
# set(PROTO_SRCS ${PROTO_FILES})

# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRCS})
add_executable(user_service_pvd 
    provider/user_service_provider.cc 
    provider/user.pb.cc 
    provider/user.pb.h)
# protobuf_generate(
#     TARGET user_service_pvd
#     LANGUAGE cpp
#     PROTOC_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/provider
#     PROTOS ${PROTO_SRCS}
# )
target_link_libraries(user_service_pvd rpc)

add_executable(user_service_req 
    requester/user_service_requester.cc 
    requester/user.pb.cc 
    requester/user.pb.h)
target_link_libraries(user_service_req rpc)