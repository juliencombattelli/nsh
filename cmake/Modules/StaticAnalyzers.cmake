# Cppcheck was supported in CMake 3.10 but Clang-tidy make it only in 3.12
cmake_minimum_required(VERSION 3.12)

option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
if(ENABLE_CPPCHECK)
    find_program(CPPCHECK cppcheck)
    if(CPPCHECK)
        set(CMAKE_CXX_CPPCHECK 
            ${CPPCHECK} 
                --suppress=missingInclude 
                --enable=all
                --inconclusive
        )
    else()
        message(SEND_ERROR "cppcheck requested but executable not found")
    endif()
endif()

option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANGTIDY 
        NAMES 
            clang-tidy clang-tidy-12 clang-tidy-11 clang-tidy-10 clang-tidy-9 clang-tidy-8 clang-tidy-7 clang-tidy-6.0
    )
    if(CLANGTIDY)
        set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY})
        set(CMAKE_C_CLANG_TIDY ${CLANGTIDY})
    else()
        message(SEND_ERROR "clang-tidy requested but executable not found")
    endif()
endif()

option(ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF)
if(ENABLE_INCLUDE_WHAT_YOU_USE)
    find_program(INCLUDE_WHAT_YOU_USE include-what-you-use)
    if(INCLUDE_WHAT_YOU_USE)
        set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE})
    else()
        message(SEND_ERROR "include-what-you-use requested but executable not found")
    endif()
endif()