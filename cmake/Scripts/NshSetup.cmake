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
include(GoogleTest)
include(StaticAnalyzers)

