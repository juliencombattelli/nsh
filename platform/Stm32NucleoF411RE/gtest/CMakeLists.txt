cmake_minimum_required(VERSION 3.14)
project(NshSTM32GTestMain LANGUAGES C CXX ASM)

nsh_add_library(stm32-gtest-main STATIC
    main.cpp
)
target_compile_features(stm32-gtest-main
    PRIVATE
        cxx_std_17
)
target_include_directories(stm32-gtest-main
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(stm32-gtest-main
    PUBLIC
        Nsh::GTest
    PRIVATE
        Nsh::Bsp
        HAL::STM32::F4::RCC
        HAL::STM32::F4::GPIO
        HAL::STM32::F4::CORTEX
        HAL::STM32::F4::UART
)

add_library(Nsh::GTestMain ALIAS stm32-gtest-main)