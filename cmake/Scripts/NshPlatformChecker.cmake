# Distributed under the MIT License. See accompanying LICENSE file for details.

#[=======================================================================[.rst:
NshPlatformChecker
------------------

This module provides the function ``nsh_check_platform`` verifying if the current
environment has all needed variables, functions and targets defined.
The ``nsh_check_platform`` function must be called AFTER the inclusion of the
platform file used.

The ``nsh_check_platform`` function checks if the following targets exist:

``Nsh::Platform``
  Alias or imported target providing all compile options, link options, and library 
  dependencies every executable and library must use to be built for the considered platform.

``Nsh::Platform::GTest``
  Alias or imported target providing GTest and GMock library built for the considered platform.

``Nsh::Platform::GTestMain``
  Alias or imported target providing GTest main entry point built for the considered platform.

``nsh_platform_add_executable``
  Function adding an executable to the project using the specified source files.
  This function is used to inject platform specific behavior to build an executable.
  Its signature is the same as CMake's `add_executable
  <https://cmake.org/cmake/help/latest/command/add_executable.html>`_.

``nsh_platform_add_library``
  Function adding a library to the project using the specified source files.
  This function is used to inject platform specific behavior to build a library.
  Its signature is the same as CMake's `add_library
  <https://cmake.org/cmake/help/latest/command/add_library.html>`_.

``CACHE{NSH_GTEST_PATCH_COMMAND}``
  Command executed as the patch step of GTest installation. It must be compatible with
  `FetchContent_Declare <https://cmake.org/cmake/help/latest/module/FetchContent.html#command:fetchcontent_declare>`_'s
  ``PATCH_COMMAND`` option.

#]=======================================================================]

# message(CHECK_* is available since CMake 3.17
cmake_minimum_required(VERSION 3.17)

function(_nsh_assert_variable_exists VARIABLE)
    message(CHECK_START "Looking for variable ${VARIABLE}")
    if(DEFINED ${VARIABLE})
        message(CHECK_PASS "found")
    else()
        message(CHECK_FAIL "not found")
        list(APPEND checks_failed ${VARIABLE})
        set(checks_failed ${checks_failed} PARENT_SCOPE)
    endif()
endfunction()

function(_nsh_assert_target_exists TARGET)
    message(CHECK_START "Looking for target ${TARGET}")
    if(TARGET ${TARGET})
        message(CHECK_PASS "found")
    else()
        message(CHECK_FAIL "not found")
        list(APPEND checks_failed ${TARGET})
        set(checks_failed ${checks_failed} PARENT_SCOPE)
    endif()
endfunction()

function(_nsh_assert_command_exists COMMAND)
    message(CHECK_START "Looking for command ${COMMAND}")
    if(COMMAND ${COMMAND})
        message(CHECK_PASS "found")
    else()
        message(CHECK_FAIL "not found")
        list(APPEND checks_failed ${COMMAND})
        set(checks_failed ${checks_failed} PARENT_SCOPE)
    endif()
endfunction()


function(nsh_check_platform)
    message(CHECK_START "Checking platform \"$CACHE{NSH_PLATFORM_NAME}\"")
    list(APPEND CMAKE_MESSAGE_INDENT "  ")
    unset(checks_failed)

    if(ENABLE_TESTS)
        _nsh_assert_variable_exists(NSH_GTEST_PATCH_COMMAND)
    endif()
    _nsh_assert_target_exists(Nsh::Platform)
    if(ENABLE_TESTS)
        _nsh_assert_target_exists(Nsh::Platform::GTest)
        _nsh_assert_target_exists(Nsh::Platform::GTestMain)
    endif()
    _nsh_assert_command_exists(nsh_platform_add_executable)
    _nsh_assert_command_exists(nsh_platform_add_library)

    list(POP_BACK CMAKE_MESSAGE_INDENT)
    if(checks_failed)
        message(CHECK_FAIL "not valid")
        string(REPLACE ";"  ", " checks "${checks_failed}")
        message(FATAL_ERROR "Platform \"$CACHE{NSH_PLATFORM_NAME}\" is not valid (missing: ${checks})")
    else()
        message(CHECK_PASS "valid")
    endif()
endfunction()
