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

function(stm32_target_add_flash TARGET)
    find_program(OPENOCD NAMES openocd PATHS ${TOOLCHAIN_BIN_PATH})
    if(OPENOCD STREQUAL OPENOCD-NOTFOUND)
        message(WARNING "openocd not found. Flash targets will not be generated.")
    else()
        string(REPLACE "@binary@" "\"${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.bin\"" OPENOCD_COMMAND ${OPENOCD_COMMAND})
        add_custom_target(${TARGET}-flash
            COMMAND echo "${OPENOCD} -f '${OPENOCD_CONF_FILE}' -c '${OPENOCD_COMMAND}'"
            DEPENDS ${TARGET}-bin
            VERBATIM
        )
    endif()
endfunction()

function(nsh_add_executable TARGET)
    add_executable(${TARGET} ${ARGN})
    stm32_target_add_size(${TARGET})
    stm32_target_add_bin(${TARGET})
    stm32_target_add_flash(${TARGET})
endfunction()