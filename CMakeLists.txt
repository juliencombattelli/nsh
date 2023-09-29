cmake_minimum_required(VERSION 3.17)
project(Nsh
    VERSION 1.4.0
    DESCRIPTION "A lightwheight shell (Nano-SHell) targeting embedded systems"
    HOMEPAGE_URL "https://github.com/juliencombattelli/nsh"
    LANGUAGES C CXX ASM
)

include(CMakeDependentOption)

option(ENABLE_TESTS "Download GTest and build the Nsh test suite" ON)
cmake_dependent_option(ENABLE_COVERAGE "Enable test coverage report generation" OFF "ENABLE_TESTS" OFF)
option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
option(ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF)
option(ENABLE_SIZE_REPORT "Generate a report of the binary size with and without the supported features" OFF)

# Include setup script defining and verifying the targeted platform
include(cmake/Scripts/NshSetup.cmake)

set(CMAKE_CXX_EXTENSIONS OFF)

configure_file(
    ${PROJECT_SOURCE_DIR}/include/nsh/nsh_version.h.in
    ${PROJECT_BINARY_DIR}/include/nsh/nsh_version.h
)

# Main library with sources and platform specific options
nsh_add_library(nsh
    ${PROJECT_SOURCE_DIR}/src/nsh.c
    ${PROJECT_SOURCE_DIR}/src/nsh_cmd.c
    ${PROJECT_SOURCE_DIR}/src/nsh_cmd_array.c
    ${PROJECT_SOURCE_DIR}/src/nsh_cmd_builtins.c
    ${PROJECT_SOURCE_DIR}/src/nsh_io_plugin_default.c
    ${PROJECT_SOURCE_DIR}/src/nsh_history.c
    ${PROJECT_SOURCE_DIR}/src/nsh_line_buffer.c
)
target_include_directories(nsh
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/include
)
target_compile_features(nsh
    PUBLIC
        c_std_11
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
    include(GoogleTest)
    include(Coverage)
    add_subdirectory(test)
endif()

if(ENABLE_SIZE_REPORT)
    if(
        (CMAKE_HOST_UNIX)
            AND
        (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
            AND
        (CMAKE_GENERATOR MATCHES "Makefile" OR CMAKE_GENERATOR MATCHES "Ninja")
    )
        add_subdirectory(tools/size-report)
    else()
        message(WARNING
            " Size report requested but usable only on Unix-like platforms with GCC or Clang,\n"
            "   when Ninja or a Makefile generator is used.\n"
            " Size report generation will be disabled."
        )
    endif()
endif()
