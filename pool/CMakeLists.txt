add_target(pool SHARED)
target_link_libraries(pool mysqlclient hiredis fmt::fmt)

add_subdirectory(tests)