set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(NSH_GTEST_PATCH_COMMAND "")

add_library(Nsh::Bsp INTERFACE IMPORTED)

add_library(Nsh::GTest INTERFACE IMPORTED)

function(nsh_add_executable)
    add_executable(${ARGV})
endfunction()

function(nsh_add_library)
    add_library(${ARGV})
endfunction()
