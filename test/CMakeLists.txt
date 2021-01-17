enable_testing()

if(NSH_PLATFORM_NAME STREQUAL Stm32NucleoF411RE)
    add_subdirectory(Stm32NucleoF411RE)
endif()

nsh_add_executable(testgtest test.cpp)
target_link_libraries(testgtest PRIVATE gtest)