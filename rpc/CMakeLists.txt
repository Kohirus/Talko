add_target(rpc SHARED)
target_link_libraries(rpc json net ${Protobuf_LIBRARIES})

add_subdirectory(tests)