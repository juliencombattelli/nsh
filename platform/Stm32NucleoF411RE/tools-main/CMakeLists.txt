cmake_minimum_required(VERSION 3.14)
project(NshSTM32ToolsMain LANGUAGES C CXX ASM)

nsh_platform_add_library(tools-main STATIC
    tools-main.cpp
)
target_compile_features(tools-main
    PRIVATE
        cxx_std_17
)
target_include_directories(tools-main
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(tools-main
    PRIVATE
        Nsh::Platform
        HAL::STM32::F4::RCC
        HAL::STM32::F4::GPIO
        HAL::STM32::F4::CORTEX
        HAL::STM32::F4::UART
)

add_library(Nsh::Platform::ToolsMain ALIAS tools-main)