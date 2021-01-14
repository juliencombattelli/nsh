set(OPENOCD_CONF_FILE board/st_nucleo_f4.cfg)
set(OPENOCD_COMMAND "program @binary@ preverify verify reset exit 0x08000000")

# TODO move openocd related stuff into a OpenOCD platform file to be included here
find_package(OpenOCD 0.10.0)
if(OPENOCD_FOUND AND NSH_STM32_TARGET)
    set(NSH_FLASH_TARGET ON)
else()
    set(NSH_FLASH_TARGET OFF)
endif()

if(NOT NSH_FLASH_TARGET)
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
    # TODO
endfunction()

