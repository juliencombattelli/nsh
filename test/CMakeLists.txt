enable_testing()

add_subdirectory(units)

if(UNIX)
    add_subdirectory(simple_shell)
endif()