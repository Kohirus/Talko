add_executable(formatter_test formatter_test.cc)
target_link_libraries(formatter_test log GTest::GTest GTest::Main)
add_test(NAME formatter_test COMMAND formatter_test)

add_executable(console_test console_test.cc)
target_link_libraries(console_test log)

add_executable(file_test file_test.cc)
target_link_libraries(file_test log)

add_executable(rotating_file_test rotating_file_test.cc)
target_link_libraries(rotating_file_test log)