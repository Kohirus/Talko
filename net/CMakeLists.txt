add_target(net SHARED)
target_link_libraries(net PUBLIC log)

add_subdirectory(tests)