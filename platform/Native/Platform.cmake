set(NSH_GTEST_PATCH_COMMAND "")
# TODO check gnu size availability
set(CMAKE_SIZE "size")

add_library(Nsh::Platform INTERFACE IMPORTED GLOBAL)

add_library(Nsh::Platform::GTest INTERFACE IMPORTED GLOBAL)
target_link_libraries(Nsh::Platform::GTest INTERFACE gtest gmock)

add_library(Nsh::Platform::GTestMain INTERFACE IMPORTED GLOBAL)
target_link_libraries(Nsh::Platform::GTestMain INTERFACE gtest_main)

function(nsh_platform_add_executable)
    add_executable(${ARGV})
endfunction()

function(nsh_platform_add_library)
    add_library(${ARGV})
endfunction()

function(nsh_platform_add_test)
    add_test(${ARGV})
endfunction()

