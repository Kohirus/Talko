file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cc include/*.h)
add_library(utils SHARED ${srcs})
target_include_directories(utils PUBLIC include)

add_subdirectory(tests)