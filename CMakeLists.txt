cmake_minimum_required(VERSION 3.17)
project(nsh LANGUAGES C CXX ASM)

include(cmake/Scripts/NshSetup.cmake)

nsh_add_library(nsh
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
)

install(
    TARGETS simple_shell
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(ENABLE_TESTS)
    add_subdirectory(test)
endif()

if(UNIX)
    add_executable(send_cmds test/send_cmds.c)
    add_test(NAME simple_shell_test COMMAND bash -c "$<TARGET_FILE:send_cmds> -d1000 help exit | $<TARGET_FILE:simple_shell>")
endif()