add_target(log SHARED)
target_link_libraries(log PUBLIC utils pool fmt::fmt)

add_subdirectory(tests)