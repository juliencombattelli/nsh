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
    add_custom_target(${TARGET}-bin ALL 
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET}> ${TARGET}.bin
        DEPENDS ${TARGET}-size 
        VERBATIM
    )
endfunction()

function(nsh_add_executable TARGET)
    add_executable(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PRIVATE STM32::NoSys)
    stm32_target_add_size(${TARGET})
    stm32_target_add_bin(${TARGET})
endfunction()