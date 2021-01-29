cmake_minimum_required(VERSION 3.14)
project(Stm32NucleoF411RE_main LANGUAGES C CXX ASM)

nsh_add_executable(Stm32NucleoF411RE_main
    main.cpp
    syscalls.c
)
target_compile_features(Stm32NucleoF411RE_main
    PRIVATE
        cxx_std_17
)
target_include_directories(Stm32NucleoF411RE_main
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(Stm32NucleoF411RE_main
    PRIVATE 
        Nsh::Bsp
        STM32::Nano
        HAL::STM32::F4::RCC
        HAL::STM32::F4::GPIO
        HAL::STM32::F4::CORTEX
        HAL::STM32::F4::UART
        gtest
)