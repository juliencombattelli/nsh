enable_testing()

nsh_add_executable(testgtest test.cpp)
target_link_libraries(testgtest PRIVATE gtest)