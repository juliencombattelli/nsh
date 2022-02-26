# FetchContent_MakeAvailable was added in CMake 3.14
cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
    fixed_string
    GIT_REPOSITORY https://github.com/unterumarmung/fixed_string
    GIT_TAG master
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(fixed_string)