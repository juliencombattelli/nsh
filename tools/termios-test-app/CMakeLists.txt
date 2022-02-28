nsh_add_intermediate_tool(termios-test-app termios-test-app.cpp)
target_compile_definitions(termios-test-app
    PUBLIC
        cxx_std_17
)
