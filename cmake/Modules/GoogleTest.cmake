# FetchContent_MakeAvailable was added in CMake 3.14
cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         v1.14.0
    GIT_SHALLOW     TRUE
    PATCH_COMMAND   ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/GoogleTest/gtest_patch.cmake
)

FetchContent_MakeAvailable(googletest)

target_link_libraries(gtest PUBLIC Nsh::Platform)
target_link_libraries(gtest PRIVATE Nsh::Platform::GTest)
