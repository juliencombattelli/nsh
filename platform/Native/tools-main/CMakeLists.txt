cmake_minimum_required(VERSION 3.14)
project(NshNativeToolsMain LANGUAGES C CXX)

nsh_platform_add_library(tools-main STATIC
    main.cpp
)
target_compile_features(tools-main
    PRIVATE
        cxx_std_17
)

add_library(Nsh::Platform::ToolsMain ALIAS tools-main)