set(NSH_GTEST_PATCH_COMMAND "")

add_library(Nsh::Bsp INTERFACE IMPORTED)

add_library(Nsh::GTest INTERFACE IMPORTED)

function(nsh_add_executable)
    add_executable(${ARGV})
endfunction()

function(nsh_add_library)
    add_library(${ARGV})
endfunction()