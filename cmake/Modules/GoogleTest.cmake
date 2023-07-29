# FetchContent_MakeAvailable was added in CMake 3.14
cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         40412d85124f7c6f3d88454583c4633e5e10fc8c # TODO Update to 1.14 when released
    GIT_SHALLOW     FALSE # Revert to TRUE when using a branch/tag instead of commit hash
    PATCH_COMMAND   ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/GoogleTest/gtest_patch.cmake
)

FetchContent_MakeAvailable(googletest)

target_link_libraries(gtest PUBLIC Nsh::Platform)
target_link_libraries(gtest PRIVATE Nsh::Platform::GTest)
