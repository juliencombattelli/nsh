# Distributed under the MIT License. See accompanying LICENSE file for details.

#[=======================================================================[.rst:
gtest-patch
-----------

Patch googletest repository.
Here is the list of the patches applied with the rationale behing them:

``Use #if to check filesystem support``
  Replace #ifdef with #if to check GTEST_HAS_FILE_SYSTEM since the flag is always defined and #ifdef
  does not work as intended here.

#]=======================================================================]

find_package(Git REQUIRED)

execute_process(
    COMMAND ${GIT_EXECUTABLE} reset --hard
)

execute_process(
    COMMAND ${GIT_EXECUTABLE} apply ${CMAKE_CURRENT_LIST_DIR}/0001-Use-if-to-check-filesystem-support.patch
)
