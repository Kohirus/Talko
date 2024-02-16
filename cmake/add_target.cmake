macro(add_target name type)
    file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cc src/*.h)
    if ("${type}" MATCHES "EXECUTABLE")
        add_executable(${name} ${srcs})
    else()
        add_library(${name} ${type} ${srcs})
    endif()
    target_include_directories(${name} PUBLIC include)
endmacro()