nsh_add_executable(utests
    test_nsh_cmd.cpp
)
target_link_libraries(utests
    PRIVATE
        Nsh::Nsh
        Nsh::Platform::GTest
        Nsh::Platform::GTestMain
)

nsh_add_test(NAME utests COMMAND utests)