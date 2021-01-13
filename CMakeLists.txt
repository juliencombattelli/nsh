cmake_minimum_required(VERSION 3.17)
project(nsh LANGUAGES C)

include(cmake/Scripts/NshSetup.cmake)

set(OPENOCD_CONF_FILE board/st_nucleo_f4.cfg)
set(OPENOCD_COMMAND "init\;reset halt\;flash write_image erase @binary@ 0x08000000\;reset\;shutdown")

add_library(nsh 
    src/nsh.c
    src/nsh_cmd.c
    src/nsh_io.c
    src/nsh_cmd_builtins.c
    src/nsh_history.c
    src/nsh_line_buffer.c
)
target_include_directories(nsh
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_features(nsh
    PUBLIC
        c_restrict
)

nsh_add_executable(simple_shell test/simple_shell.c)
target_link_libraries(simple_shell
    PRIVATE
        nsh
        $<$<STREQUAL:${NSH_PLATFORM},STM32>:STM32::NoSys>
)

install(
    TARGETS simple_shell
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(NSH_PLATFORM STREQUAL Native)
    add_executable(send_cmds test/send_cmds.c)
    enable_testing()
    add_test(NAME simple_shell_test COMMAND bash -c "$<TARGET_FILE:send_cmds> -d1000 help exit | $<TARGET_FILE:simple_shell>")
endif()