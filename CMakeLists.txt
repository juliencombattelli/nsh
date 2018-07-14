cmake_minimum_required (VERSION 3.1)
project (nsh LANGUAGES C)

set (NSH_LIBRARY_NAME ${CMAKE_PROJECT_NAME})
set (NSH_HEADER_DIR include)
set (NSH_SOURCE_DIR src)

list (APPEND NSH_SOURCE_FILES 
    ${NSH_SOURCE_DIR}/shell.c
    ${NSH_SOURCE_DIR}/shell_cmd.c      
    ${NSH_SOURCE_DIR}/shell_io.c
    ${NSH_SOURCE_DIR}/shell_cmd_builtins.c
    ${NSH_SOURCE_DIR}/shell_history.c
    ${NSH_SOURCE_DIR}/shell_line_buffer.c
)

add_library (${NSH_LIBRARY_NAME} ${NSH_SOURCE_FILES})

target_include_directories (${NSH_LIBRARY_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:${NSH_HEADER_DIR}>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${NSH_HEADER_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/${NSH_SOURCE_DIR}
)

target_compile_features (${NSH_LIBRARY_NAME} PUBLIC c_restrict)

add_executable (simple_shell test/simple_shell.c)
target_link_libraries (simple_shell ${NSH_LIBRARY_NAME})

add_executable (send_cmds test/send_cmds.c)
enable_testing ()

add_test (NAME simple_shell_test COMMAND bash -c "$<TARGET_FILE:send_cmds> -d1000 help exit | $<TARGET_FILE:simple_shell>")
    

