cmake_minimum_required(VERSION 3.17)
project(nsh LANGUAGES C CXX ASM)

include(cmake/Modules/Option.cmake)
nsh_clear_option_list()

nsh_add_option(NSH_ENABLE_TESTS "Download GTest and build the Nsh test suite" ON)
nsh_add_dependent_option(NSH_ENABLE_COVERAGE "Enable test coverage report generation" OFF "NSH_ENABLE_TESTS" OFF)
nsh_add_option(NSH_ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
nsh_add_option(NSH_ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
nsh_add_option(NSH_ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF)
nsh_add_option(NSH_ENABLE_SIZE_REPORT "Generate a report of the binary size with and without the supported features" OFF)

# Include setup script defining and verifying the targeted platform
include(cmake/Scripts/NshSetup.cmake)

set(CMAKE_CXX_EXTENSIONS OFF)

if(NSH_ENABLE_TESTS)
    enable_testing()
    include(GoogleTest)
    include(Coverage)
    if(UNIX)
        add_subdirectory(tools/simple_shell)
    endif()
endif()

add_subdirectory(nsh)
add_subdirectory(nshpp)

if(NSH_ENABLE_SIZE_REPORT)
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
            "   when Ninja or a Makefile generator are used.\n"
            " Size report generation will be disabled."
        )
    endif()
endif()

# Print the current NSH configuration.
# Useful to let the user know the configuration variables without reading the code or using third-party tools.
# Must be done last to ensure the function catches all options that may be defined in the project.
nsh_print_options()
