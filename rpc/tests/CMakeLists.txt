add_executable(app_test app_test.cc)
target_link_libraries(app_test rpc)

add_executable(user_service_pvd 
    provider/user_service_provider.cc 
    provider/user.pb.cc 
    provider/user.pb.h)
target_link_libraries(user_service_pvd rpc)

add_executable(user_service_req 
    requester/user_service_requester.cc 
    requester/user.pb.cc 
    requester/user.pb.h)
target_link_libraries(user_service_req rpc)