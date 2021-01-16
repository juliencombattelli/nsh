include(FetchContent)

FetchContent_Declare(
    stm32-cmake
    GIT_REPOSITORY  https://github.com/ObKo/stm32-cmake
    GIT_TAG         master
    GIT_SHALLOW     TRUE
    PATCH_COMMAND   ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/Stm32Gcc/stm32-cmake_patch.cmake
)
FetchContent_MakeAvailable(stm32-cmake)

include(${stm32-cmake_SOURCE_DIR}/cmake/stm32_gcc.cmake)

# Repeat find_program with REQUIRED option (missing in stm32_gcc.cmake...)
find_program(CMAKE_C_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH} REQUIRED)
find_program(CMAKE_CXX_COMPILER NAMES ${STM32_TARGET_TRIPLET}-g++ PATHS ${TOOLCHAIN_BIN_PATH} REQUIRED)
find_program(CMAKE_ASM_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH} REQUIRED)

set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")

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