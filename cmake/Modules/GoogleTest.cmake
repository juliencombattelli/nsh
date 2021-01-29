# FetchContent_MakeAvailable was added in CMake 3.14
cmake_minimum_required(VERSION 3.14)

option(ENABLE_TESTS "Download GTest and build the Nsh test suite" ON)
if(ENABLE_TESTS)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY  https://github.com/google/googletest.git
        GIT_TAG         release-1.10.0
        GIT_SHALLOW     TRUE
        PATCH_COMMAND   ${NSH_GTEST_PATCH_COMMAND}
    )

    FetchContent_MakeAvailable(googletest)

    target_link_libraries(gtest PUBLIC Nsh::Bsp)
    target_link_libraries(gtest PRIVATE Nsh::GTest)
endif()