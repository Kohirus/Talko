add_executable(formatter_test formatter_test.cc)
target_link_libraries(formatter_test log GTest::GTest GTest::Main)
add_test(NAME formatter_test COMMAND formatter_test)