if(UNIX)

nsh_add_tool(nsh-termios-test-app main.cpp)
target_link_libraries(nsh-termios-test-app
    PUBLIC
        Nsh::Nsh
        termios-test-app
)

nsh_add_test(
    NAME termios-test-app_test_default_cmds
    # Send: "help<ENTER>", "exit<ENTER>"
    # Expected: command "help" then "exit" are executed
    COMMAND bash -c "echo -e 'help\\nexit\\n' | $<TARGET_FILE:termios-test-app>"
)
nsh_add_test(
    NAME termios-test-app_test_autocomplete
    # Send: "exi<TAB><ENTER>"
    # Expected: command "exit" is executed
    COMMAND bash -c "echo -e 'exi\\t\\n' | $<TARGET_FILE:termios-test-app>"
)
nsh_add_test(
    NAME termios-test-app_test_backspace
    # Send: "hell<BS>p<ENTER>", "exit<ENTER>"
    # Expected: command "help" is executed, then exit
    COMMAND bash -c "echo -e 'hell\\bp\\nexit\\n' | $<TARGET_FILE:termios-test-app>"
)
nsh_add_test(
    NAME termios-test-app_test_history
    # Send: "hello<ENTER>", "holla<ENTER>", "<UP><UP><UP><DOWN><DOWN><ENTER>", "exit<ENTER>"
    # Expected: command "hello", then "holla", then "holla" are executed, then exit
    COMMAND bash -c "echo -e 'hello\\nholla\\n\\x1b[A\\x1b[A\\x1b[A\\x1b[B\\x1b[B\\nexit\\n' | $<TARGET_FILE:termios-test-app>"
)
nsh_add_test(
    NAME termios-test-app_test_split_cmdline
    # Send: "It is not a bug<ENTER>", "exit<ENTER>"
    # Expected: command "It" is executed with arguments ["is", "not", "a", "bug"], then exit
    COMMAND bash -c "echo -e 'It is not a bug\\nexit\\n' | $<TARGET_FILE:termios-test-app>"
)
nsh_add_test(
    NAME termios-test-app_test_long_line
    # Send: "<A really long input that does not fit in line buffer><ENTER>", "exit<ENTER>"
    # Expected: line buffer maximum capacity reached warning message, then exit
    COMMAND bash -c "echo -e 'Writing the first 90 percent of a computer program takes 90 percent of the time. The remaining ten percent also takes 90 percent of the time and the final touches also take 90 percent of the time.\\nexit' | $<TARGET_FILE:termios-test-app>"
)
nsh_add_test(
    NAME termios-test-app_test_long_arg
    # Send: "<An argument too long to fit in buffer><ENTER>", "exit<ENTER>"
    # Expected: too long argument warning message, then exit
    COMMAND bash -c "echo -e 'ThisIsAVeryLongArgument\\nexit\\n' | $<TARGET_FILE:termios-test-app>"
)
nsh_add_test(
    NAME termios-test-app_test_too_many_arg
    # Send: "<A command with far too much arguments><ENTER>", "exit<ENTER>"
    # Expected: too many arguments warning message, then exit
    COMMAND bash -c "echo -e 'a b c d e f g h i j k l m n o p q r s t u v w x y z 0 1 2 3 4 5 6 7 8 9\\nexit\\n' | $<TARGET_FILE:termios-test-app>"
)
nsh_add_test(
    NAME termios-test-app_test_unsupported_input
    # Send: "<LEFT><ENTER>", "exit<ENTER>"
    # Expected: input ignored, then exit
    COMMAND bash -c "echo -e '\\x1b[C\\nexit\\n' | $<TARGET_FILE:termios-test-app>"
)
nsh_add_test(
    NAME termios-test-app_test_null_cmd
    # Send: "null<ENTER>", "exit<ENTER>"
    # Expected: null command is NOT executed (because handled by a null pointer), then exit
    COMMAND bash -c "echo -e 'null\\nexit\\n' | $<TARGET_FILE:termios-test-app>"
)

endif()
