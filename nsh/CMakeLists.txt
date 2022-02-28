# Main library with sources and platform specific options
nsh_add_library(nsh
    ${CMAKE_CURRENT_LIST_DIR}/src/nsh.c
    ${CMAKE_CURRENT_LIST_DIR}/src/nsh_cmd.c
    ${CMAKE_CURRENT_LIST_DIR}/src/nsh_cmd_array.c
    ${CMAKE_CURRENT_LIST_DIR}/src/nsh_cmd_builtins.c
    ${CMAKE_CURRENT_LIST_DIR}/src/nsh_history.c
    ${CMAKE_CURRENT_LIST_DIR}/src/nsh_io_plugin_default.c
    ${CMAKE_CURRENT_LIST_DIR}/src/nsh_line_buffer.c
)
target_include_directories(nsh
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/include
)
target_compile_features(nsh
    PRIVATE
        c_std_11
)

add_library(Nsh::Nsh ALIAS nsh)

install(
    TARGETS nsh
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(NSH_ENABLE_TESTS)
    add_subdirectory(test/units)
    add_subdirectory(test/integrations)
endif()
