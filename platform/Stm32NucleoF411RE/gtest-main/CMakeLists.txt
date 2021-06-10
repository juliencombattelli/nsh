cmake_minimum_required(VERSION 3.14)
project(NshSTM32GTestMain LANGUAGES C CXX ASM)

nsh_add_library(stm32-gtest-main STATIC
    gtest-main.cpp
)
target_compile_features(stm32-gtest-main
    PRIVATE
        cxx_std_17
)
target_link_libraries(stm32-gtest-main
    PUBLIC
        Nsh::Platform::GTest
)

add_library(Nsh::Platform::GTestMain INTERFACE IMPORTED GLOBAL)
target_link_libraries(Nsh::Platform::GTestMain
    INTERFACE
        Nsh::Platform::ToolsMain
        stm32-gtest-main
)
