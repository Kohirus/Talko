file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cc include/*.h)
add_library(log SHARED ${srcs})
target_include_directories(log PUBLIC include)
target_link_libraries(log PUBLIC utils fmt::fmt)

add_subdirectory(tests)