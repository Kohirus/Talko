add_executable(echo_server echo_server.cc)
target_link_libraries(echo_server net)

add_executable(timer_test timer_test.cc)
target_link_libraries(timer_test net)

add_executable(cmd_server cmd_server.cc)
target_link_libraries(cmd_server net)