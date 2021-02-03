# Prepend Modules folder to CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../Modules ${CMAKE_MODULE_PATH})

# Define the nsh_set_platform function setting the NSH_PLATFORM_NAME cache variable
function(nsh_set_platform PLATFORM)
    set(NSH_PLATFORM_NAME ${PLATFORM} CACHE INTERNAL "Current targeted platform.")
endfunction()

# Set platform to "Native" if not cross-compiling
if (NOT CMAKE_CROSSCOMPILING)
    nsh_set_platform(Native)
endif()

# Include platform file
include(${CMAKE_CURRENT_LIST_DIR}/../../platform/${NSH_PLATFORM_NAME}/Platform.cmake)

# Check if the selected platform is valid
include(${CMAKE_CURRENT_LIST_DIR}/NshPlatformChecker.cmake)
nsh_check_platform(${NSH_PLATFORM_NAME})

# Include additional modules
include(StaticAnalyzers)
include(Warnings)

# Define Nsh specific add_library wrapper
function(nsh_add_library TARGET)
    nsh_platform_add_library(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PUBLIC Nsh::Platform)
    target_project_warnings(${TARGET})
endfunction()

# Define Nsh specific add_executable wrapper
function(nsh_add_executable TARGET)
    nsh_platform_add_executable(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PUBLIC Nsh::Platform)
    target_project_warnings(${TARGET})
endfunction()

# Define Nsh specific add_test wrapper
function(nsh_add_test)
    nsh_platform_add_test(${ARGV})
endfunction()