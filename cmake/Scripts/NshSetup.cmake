# Prepend Modules folder to CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../Modules ${CMAKE_MODULE_PATH})

# Define the nsh_set_platform function setting the NSH_PLATFORM_NAME cache variable
function(nsh_set_platform PLATFORM)
    set(NSH_PLATFORM_NAME ${PLATFORM} CACHE INTERNAL "Current targeted platform.")
endfunction()

# Set platform to "Native" if not cross-compiling
if(NOT CMAKE_CROSSCOMPILING)
    nsh_set_platform(Native)
endif()

# Include additional modules
include(ForceColoredOutput)
include(Sanitizers)
include(StaticAnalyzers)
include(Warnings)

# Define Nsh specific add_library wrapper
function(nsh_add_library TARGET)
    nsh_platform_add_library(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PUBLIC Nsh::Platform)
    target_project_warnings(${TARGET})
    target_enable_sanitizers(${TARGET})
endfunction()

# Define Nsh specific add_executable wrapper
function(nsh_add_executable TARGET)
    nsh_platform_add_executable(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PUBLIC Nsh::Platform)
    target_project_warnings(${TARGET})
    target_enable_sanitizers(${TARGET})
    set_property(TARGET ${TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

    # include(CheckIPOSupported)
    # check_ipo_supported(RESULT ipo_supported OUTPUT error)
    # if(ipo_supported)
    #     message(STATUS "IPO / LTO enabled")
    #     set_property(TARGET example PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    # else()
    #     message(STATUS "IPO / LTO not supported: <${error}>")
    # endif()
endfunction()

# Define Nsh specific add_test wrapper
function(nsh_add_test)
    nsh_platform_add_test(${ARGV})
endfunction()

# Define Nsh wrapper to add a platform-agnostic tool
function(nsh_add_tool TARGET)
    nsh_add_executable(${TARGET} ${ARGN})
    target_compile_features(${TARGET} PRIVATE cxx_std_17)
    target_link_libraries(${TARGET} PRIVATE Nsh::Platform::ToolsMain)
endfunction()

# Include platform file
include(${CMAKE_CURRENT_LIST_DIR}/../../platform/${NSH_PLATFORM_NAME}/Platform.cmake)

# Check if the selected platform is valid
include(${CMAKE_CURRENT_LIST_DIR}/NshPlatformChecker.cmake)
nsh_check_platform(${NSH_PLATFORM_NAME})
