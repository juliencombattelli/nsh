nsh_add_executable(simple_shell simple_shell.c)
target_link_libraries(simple_shell
    PRIVATE
        nsh
)

add_executable(send_cmds send_cmds.c)
add_test(
    NAME simple_shell_test 
    COMMAND bash -c "$<TARGET_FILE:send_cmds> -d1000 help exit | $<TARGET_FILE:simple_shell>"
)
