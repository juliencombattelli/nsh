nsh_add_executable(utests
    test_nsh_cmd.cpp
    test_nsh_cmd_array.cpp
)
target_compile_features(utests
    PRIVATE
        cxx_std_17
)
target_link_libraries(utests
    PRIVATE
        Nsh::Nsh
        Nsh::Platform::GTest
        Nsh::Platform::GTestMain
)

nsh_add_test(NAME utests COMMAND utests)