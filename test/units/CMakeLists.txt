nsh_add_executable(utests test.cpp)
target_link_libraries(utests
    PRIVATE
        Nsh::Nsh
        Nsh::GTest
        Nsh::GTestMain
)