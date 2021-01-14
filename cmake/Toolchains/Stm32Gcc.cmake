include(FetchContent)

FetchContent_Declare(
    stm32-cmake
    GIT_REPOSITORY https://github.com/ObKo/stm32-cmake
    # GIT_TAG        ${HAL_VERSION}
    GIT_PROGRESS   TRUE
)

FetchContent_GetProperties(stm32-cmake POPULATED stm32-cmake_POPULATED)
if(NOT stm32-cmake_POPULATED)
    set(FETCHCONTENT_QUIET FALSE)
    FetchContent_Populate(stm32-cmake)
endif()

include(${stm32-cmake_SOURCE_DIR}/cmake/stm32_gcc.cmake)

# Repeat find_program with REQUIRED option (missing in stm32_gcc.cmake...)
find_program(CMAKE_C_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH} REQUIRED)
find_program(CMAKE_CXX_COMPILER NAMES ${STM32_TARGET_TRIPLET}-g++ PATHS ${TOOLCHAIN_BIN_PATH} REQUIRED)
find_program(CMAKE_ASM_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH} REQUIRED)

set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")

include(${CMAKE_CURRENT_LIST_DIR}/../Scripts/NshCommon.cmake)

nsh_set_platform(STM32)

function(nsh_set_stm32_target TARGET)
    set(NSH_STM32_TARGET ${TARGET} CACHE STRING
        "Choosen STM32 hardware target. The target name must match a file in Platforms folder."
    )
endfunction()

if(NOT NSH_STM32_TARGET)
    message(STATUS "No NSH_STM32_TARGET specified, hardware-releated features (like flashing and debugging) will not be available")
    # Provide empty hardware-related functions
    function(stm32_target_flash_file)
    endfunction()
    function(stm32_target_add_flash)
    endfunction()
    function(stm32_target_add_start_debug_server)
    endfunction()
else()
    # Include hardware file that must provide the stubbed functions above
    include(${CMAKE_CURRENT_LIST_DIR}/../Platforms/${NSH_PLATFORM}/${NSH_STM32_TARGET}.cmake)
endif()

function(stm32_target_add_size TARGET)
    add_custom_target(${TARGET}-size ALL
        COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${TARGET}>
        DEPENDS ${TARGET}
        VERBATIM
    )
endfunction()

function(stm32_target_add_bin TARGET)
    add_custom_command(
        OUTPUT ${TARGET}.bin
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET}> ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.bin
        DEPENDS ${TARGET}-size
        VERBATIM
    )
    add_custom_target(${TARGET}-bin ALL 
        DEPENDS ${TARGET}.bin
    )
endfunction()

function(nsh_add_executable TARGET)
    add_executable(${TARGET} ${ARGN})
    stm32_target_add_size(${TARGET})
    stm32_target_add_bin(${TARGET})
    stm32_target_add_flash(${TARGET})
    stm32_target_add_start_debug_server(${TARGET})
endfunction()