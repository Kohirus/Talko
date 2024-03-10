cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake;${CMAKE_MODULE_PATH}")

project(Talko LANGUAGES CXX)

include(add_target)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

find_package(fmt REQUIRED)
find_package(Protobuf REQUIRED)

option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")

if(ENABLE_UNIT_TESTS)
    find_package(GTest REQUIRED)
    enable_testing()
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fPIC")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 库模块
add_subdirectory(utils) # 通用库
add_subdirectory(pool) # 池式组件
add_subdirectory(log) # 日志库
add_subdirectory(json) # JSON解析库
add_subdirectory(net) # Reactor网络库
add_subdirectory(rpc) # RPC库

# 可执行文件
add_subdirectory(registry) # 注册中心
# add_subdirectory(db_proxy_server) # 数据库代理服务器