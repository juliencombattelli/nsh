nsh_add_executable(utests test.cpp)
target_link_libraries(utests
    PRIVATE
        Nsh::Nsh
        Nsh::Platform::GTest
        Nsh::Platform::GTestMain
)

if(NSH_PLATFORM_NAME STREQUAL Native)
    add_test(NAME utests COMMAND utests)
endif()