# Distributed under the MIT License. See accompanying LICENSE file for details.

#[=======================================================================[.rst:
NshPlatformChecker
------------------

This module provides the function ``nsh_check_platform`` verifying if the current
environment has all needed variables, functions and targets defined.
The ``nsh_check_platform`` function must be called AFTER the inclusion of the
platform file used.

The ``nsh_check_platform`` function checks if the following ``IMPORTED`` targets
exist:

``Nsh::Bsp``
  Target providing all compile options, link options, and library dependencies
  every executable and libraries must use to be built for the considered platform.

``Nsh::GTest``

``nsh_add_executable``

``nsh_add_library``

``CACHE{NSH_PLATFORM_NAME}``

``CACHE{NSH_GTEST_PATCH_COMMAND}``

Example usage:

.. code-block:: cmake

  find_package(OpenOCD)
  if(OPENOCD_FOUND)
    message("OpenOCD found: ${OPENOCD_EXECUTABLE}")
  endif()

#]=======================================================================]

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
    _nsh_assert_target_exists(Nsh::Bsp)
    if(ENABLE_TESTS)
        _nsh_assert_target_exists(Nsh::GTest)
    endif()
    _nsh_assert_command_exists(nsh_add_executable)
    _nsh_assert_command_exists(nsh_add_library)

    list(POP_BACK CMAKE_MESSAGE_INDENT)
    if(checks_failed)
        message(CHECK_FAIL "not valid")
        string(REPLACE ";"  ", " checks "${checks_failed}")
        message(FATAL_ERROR "Platform \"$CACHE{NSH_PLATFORM_NAME}\" is not valid (missing: ${checks})")
    else()
        message(CHECK_PASS "valid")
    endif()
endfunction()
