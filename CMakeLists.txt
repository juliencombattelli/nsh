cmake_minimum_required(VERSION 3.17)
project(nsh LANGUAGES C CXX ASM)

option(ENABLE_TESTS "Build tests" ON)

include(cmake/Scripts/NshSetup.cmake)

nsh_add_library(nsh
    src/nsh.c
    src/nsh_cmd.c
    src/nsh_io.c
    src/nsh_cmd_builtins.c
    src/nsh_history.c
    src/nsh_line_buffer.c
)
target_include_directories(nsh
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_features(nsh
    PUBLIC
        c_restrict
)

add_library(Nsh::Nsh ALIAS nsh)

install(
    TARGETS nsh
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(ENABLE_TESTS)
    add_subdirectory(test)
endif()