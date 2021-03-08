nsh_add_executable(simple_shell simple_shell.c)
target_link_libraries(simple_shell
    PRIVATE
        nsh
)

add_test(
    NAME simple_shell_test_default_cmds
    COMMAND bash -c "echo -e 'help\\nexit' | $<TARGET_FILE:simple_shell>"
)
add_test(
    NAME simple_shell_test_autocomplete
    COMMAND bash -c "echo -e 'exi\\t' | $<TARGET_FILE:simple_shell>"
)
add_test(
    NAME simple_shell_test_backspace
    COMMAND bash -c "echo -e 'hell\\bp\\nexit' | $<TARGET_FILE:simple_shell>"
)
add_test(
    NAME simple_shell_test_history
    COMMAND bash -c "echo -e 'hello\\nholla\\n\\x1b[A\\x1b[A\\x1b[B\\nexit' | $<TARGET_FILE:simple_shell>"
)
add_test(
    NAME simple_shell_test_split_cmdline
    COMMAND bash -c "echo -e 'It is not a bug\\nexit' | $<TARGET_FILE:simple_shell>"
)
