nsh_add_executable(simple_shell simple_shell.c)
target_link_libraries(simple_shell
    PRIVATE
        nsh
)

add_test(
    NAME simple_shell_test 
    COMMAND bash -c "echo -e 'hell\\bp\\nexit' | $<TARGET_FILE:simple_shell>"
)
