stm32_fetch_cmsis(F4)
stm32_fetch_hal(F4)

find_package(CMSIS COMPONENTS STM32F4 REQUIRED)
find_package(HAL COMPONENTS STM32F4 REQUIRED)
find_package(OpenOCD 0.10.0)

set(OPENOCD_CONF_FILE board/st_nucleo_f4.cfg)
set(OPENOCD_COMMAND "program @binary@ preverify verify reset exit 0x08000000")

if(OPENOCD_FOUND)
    set(NSH_FLASH_TARGET ON)
else()
    set(NSH_FLASH_TARGET OFF)
    message(WARNING "OpenOCD not found. Flash targets will not be generated.")
endif()

function(stm32_target_flash_file FILEPATH)
    if(NSH_FLASH_TARGET)
        get_filename_component(FILENAME ${FILEPATH} NAME_WLE)
        string(REPLACE "@binary@" "${FILEPATH}" OPENOCD_COMMAND ${OPENOCD_COMMAND})
        add_custom_target(${FILENAME}-flash
            COMMAND ${OPENOCD_EXECUTABLE} -f "${OPENOCD_CONF_FILE}" -c "${OPENOCD_COMMAND}"
            DEPENDS ${FILEPATH}
            VERBATIM
        )
    endif()
endfunction()

function(stm32_target_add_flash TARGET)
    stm32_target_flash_file(${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.bin)
endfunction()

function(stm32_target_add_start_debug_server)
    add_custom_target(${TARGET}-debug
        DEPENDS $<TARGET_FILE:${TARGET}>
        # TODO check for gdb-multiarch availability
        COMMAND gdb-multiarch -ex "target extended-remote | openocd -f board/st_nucleo_f4.cfg -c \"gdb_port pipe; log_output openocd.log\"" $<TARGET_FILE:${TARGET}>
        VERBATIM USES_TERMINAL
    )
endfunction()

set(NSH_GTEST_PATCH_COMMAND "")

add_library(Nsh::Bsp INTERFACE IMPORTED)
target_link_libraries(Nsh::Bsp
    INTERFACE
        STM32::NoSys
        CMSIS::STM32::F411RE
)

function(nsh_add_library TARGET)
    add_library(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PUBLIC Nsh::Bsp)
endfunction()

function(nsh_add_executable TARGET)
    add_executable(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PUBLIC Nsh::Bsp)
    stm32_target_add_size(${TARGET})
    stm32_target_add_bin(${TARGET})
    stm32_target_add_flash(${TARGET})
    stm32_target_add_start_debug_server(${TARGET})
endfunction()

add_library(Nsh::GTest INTERFACE IMPORTED)
# No thread support on STM32F4. Using GTEST_HAS_THREAD=0 causes a "macro redefined" error...
set(gtest_disable_pthreads ON CACHE INTERNAL "")
target_compile_definitions(Nsh::GTest
    INTERFACE
        $<$<C_COMPILER_ID:GNU>:_GNU_SOURCE>
        PATH_MAX=256
        GTEST_HAS_CLONE=0
        GTEST_HAS_POSIX_RE=0
        GTEST_HAS_EXCEPTIONS=0
        GTEST_HAS_RTTI=0
        GTEST_HAS_DEATH_TEST=0
        GTEST_HAS_STREAM_REDIRECTION=0
        GTEST_HAS_TR1_TUPLE=0
)
target_compile_options(Nsh::GTest
    INTERFACE
        $<$<C_COMPILER_ID:GNU>:-Wno-psabi>
)

stm32_target_flash_file(${PROJECT_SOURCE_DIR}/bin/Nucleo_blink_led.NUCLEO_F411RE.bin)