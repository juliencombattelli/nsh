include(FetchContent)

FetchContent_Declare(
    stm32-cmake
    GIT_REPOSITORY  https://github.com/ObKo/stm32-cmake
    GIT_TAG         master
    GIT_SHALLOW     TRUE
    PATCH_COMMAND   ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/Stm32Gcc/stm32-cmake_patch.cmake
)
FetchContent_MakeAvailable(stm32-cmake)

# Problem: CMake runs toolchain files multiple times, but can't read cache variables on some runs (especialy during compiler checking from toolchain files).
# Workaround: On first run (in which cache variables are always accessible), set an intermediary environment variable, which are always preserved.
if(STM32_TOOLCHAIN_PATH)
    set(ENV{NSH_STM32_TOOLCHAIN_PATH} "${STM32_TOOLCHAIN_PATH}")
else()
    set(STM32_TOOLCHAIN_PATH "$ENV{NSH_STM32_TOOLCHAIN_PATH}")
endif()
if(STM32_TARGET_TRIPLET)
    set(ENV{NSH_STM32_TARGET_TRIPLET} "${STM32_TARGET_TRIPLET}")
else()
    set(STM32_TARGET_TRIPLET "$ENV{NSH_STM32_TARGET_TRIPLET}")
endif()

include(${stm32-cmake_SOURCE_DIR}/cmake/stm32_gcc.cmake)

# Repeat find_program with REQUIRED option (missing in stm32_gcc.cmake...)
# Could be done in stm32-cmake_patch script (given as a commented example)
find_program(CMAKE_C_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH} NO_DEFAULT_PATH REQUIRED)
find_program(CMAKE_CXX_COMPILER NAMES ${STM32_TARGET_TRIPLET}-g++ PATHS ${TOOLCHAIN_BIN_PATH} NO_DEFAULT_PATH REQUIRED)
find_program(CMAKE_ASM_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH} NO_DEFAULT_PATH REQUIRED)

set(CMAKE_C_COMPILER_TARGET ${STM32_TARGET_TRIPLET})
set(CMAKE_CXX_COMPILER_TARGET ${STM32_TARGET_TRIPLET})
set(CMAKE_ASM_COMPILER_TARGET ${STM32_TARGET_TRIPLET})

if(STM32_TOOLCHAIN_PATH AND NOT GDB_HOME)
    set(GDB_HOME ${STM32_TOOLCHAIN_PATH} CACHE STRING "")
endif()

function(stm32_target_add_size TARGET)
    add_custom_target(${TARGET}-size ALL
        COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${TARGET}>
        DEPENDS $<TARGET_FILE:${TARGET}>
        VERBATIM
    )
endfunction()

function(stm32_target_add_bin TARGET)
    add_custom_command(
        OUTPUT ${TARGET}.bin
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET}> ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.bin
        DEPENDS ${TARGET} ${TARGET}-size
        VERBATIM
    )
    add_custom_target(${TARGET}-bin ALL 
        DEPENDS ${TARGET}.bin
    )
endfunction()
