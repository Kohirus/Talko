add_executable(time_test datetime_test.cc)
target_link_libraries(time_test utils)

add_executable(os_test os_test.cc)
target_link_libraries(os_test utils)