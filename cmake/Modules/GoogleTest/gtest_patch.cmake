# Distributed under the MIT License. See accompanying LICENSE file for details.

#[=======================================================================[.rst:
gtest-patch
-----------

Patch googletest repository.
Here is the list of the patches applied with the rationale behing them:

``Always use colors if requested by the user``
  If --gtest_color=yes is provided, colors should always be used even when there
  is no filesystem support.

#]=======================================================================]

find_package(Git REQUIRED)

execute_process(
    COMMAND ${GIT_EXECUTABLE} reset --hard
)

execute_process(
    COMMAND ${GIT_EXECUTABLE} apply ${CMAKE_CURRENT_LIST_DIR}/0001-Always-use-colors-if-requested-by-user.patch
)
