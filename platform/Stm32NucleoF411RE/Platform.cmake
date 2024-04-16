stm32_fetch_cmsis(F4)
stm32_fetch_hal(F4)

find_package(CMSIS COMPONENTS STM32F4 REQUIRED)
find_package(HAL COMPONENTS STM32F4 REQUIRED)
find_package(OpenOCD 0.10.0)
find_package(GDB COMPONENTS armv7e-m)

set(OPENOCD_CONF_FILE board/st_nucleo_f4.cfg)
set(OPENOCD_COMMAND "program @binary@ verify reset exit 0x08000000")

if(OPENOCD_FOUND)
    set(NSH_FLASH_TARGET ON)
    if(GDB_FOUND)
        set(NSH_DEBUG_TARGET ON)
    else()
        set(NSH_DEBUG_TARGET OFF)
        message(WARNING "GDB not found. Debug targets will not be generated.")
    endif()
else()
    set(NSH_FLASH_TARGET OFF)
    set(NSH_DEBUG_TARGET OFF)
    message(WARNING "OpenOCD not found. Flash & Debug targets will not be generated.")
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
    if(NSH_DEBUG_TARGET)
        add_custom_target(${TARGET}-debug
            DEPENDS $<TARGET_FILE:${TARGET}>
            COMMAND ${GDB_EXECUTABLE} -ex "target extended-remote | ${OPENOCD_EXECUTABLE} -f board/st_nucleo_f4.cfg -c \"gdb_port pipe; log_output openocd.log\"" $<TARGET_FILE:${TARGET}>
            VERBATIM USES_TERMINAL
        )
    endif()
endfunction()

add_library(Nsh::Platform INTERFACE IMPORTED GLOBAL)
target_link_libraries(Nsh::Platform
    INTERFACE
        STM32::Nano
        CMSIS::STM32::F411RE
)

function(nsh_platform_add_library TARGET)
    add_library(${TARGET} ${ARGN})
endfunction()

function(nsh_platform_add_executable TARGET)
    add_executable(${TARGET} ${ARGN})
    stm32_target_add_size(${TARGET})
    stm32_target_add_bin(${TARGET})
    stm32_target_add_flash(${TARGET})
    stm32_target_add_start_debug_server(${TARGET})
endfunction()

function(nsh_platform_add_test)
    # TODO register a command:
    # - flashing the test on board
    # - getting output from serial connection
    # - determining if test passed or failed (how??)
endfunction()

add_library(Nsh::Platform::GTest INTERFACE IMPORTED GLOBAL)
# No thread support on STM32F4. Using only GTEST_HAS_PTHREAD=0 causes compile errors...
set(gtest_disable_pthreads ON CACHE INTERNAL "")
target_compile_definitions(Nsh::Platform::GTest
    INTERFACE
        $<$<C_COMPILER_ID:GNU>:_GNU_SOURCE>
        PATH_MAX=256
        # Environment related macros
        GTEST_HAS_CLONE=0
        GTEST_HAS_EXCEPTIONS=0
        GTEST_HAS_POSIX_RE=0
        GTEST_HAS_PTHREAD=0
        GTEST_HAS_RTTI=0
        GTEST_HAS_STD_WSTRING=0
        GTEST_HAS_FILE_SYSTEM=0
        GTEST_HAS_SEH=0
        GTEST_HAS_STREAM_REDIRECTION=0
        GTEST_LINKED_AS_SHARED_LIBRARY=0
        GTEST_CREATE_SHARED_LIBRARY=0
)
# Remove a GCC warning about an ABI change from GCC7.1
# See the GCC7 changelog https://gcc.gnu.org/gcc-7/changes.html and the
# associated bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=77728 for details
target_compile_options(Nsh::Platform::GTest
    INTERFACE
        "$<$<C_COMPILER_ID:GNU>:-Wno-psabi>"
)
target_link_libraries(Nsh::Platform::GTest
    INTERFACE
        gtest
        gmock
)

stm32_target_flash_file(${PROJECT_SOURCE_DIR}/bin/Nucleo_blink_led.NUCLEO_F411RE.bin)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/gtest-main)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tools-main)
