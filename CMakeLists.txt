cmake_minimum_required(VERSION 3.17)
project(nsh LANGUAGES C CXX ASM)

include(CMakeDependentOption)

option(ENABLE_TESTS "Download GTest and build the Nsh test suite" ON)
cmake_dependent_option(ENABLE_COVERAGE "Enable test coverage report generation" OFF "ENABLE_TESTS" OFF)
option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
option(ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF)

# Include setup script defining and verifying the targeted platform
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
    enable_testing()
    add_subdirectory(test)
endif()