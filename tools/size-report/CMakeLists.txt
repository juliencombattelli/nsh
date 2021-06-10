cmake_minimum_required(VERSION 3.18)
project(nsh-size-report)

function(nsh_add_size_report_target TARGET)
    # Duplicate nsh lib by creating a new one, getting sources from main nsh lib and linking to nsh-interface
    nsh_add_library(${TARGET}-lib STATIC)
    get_target_property(nsh_sources Nsh::Nsh SOURCES)
    get_target_property(nsh_include_dirs Nsh::Nsh INCLUDE_DIRECTORIES)
    get_target_property(nsh_compile_features Nsh::Nsh COMPILE_FEATURES)
    target_sources(${TARGET}-lib PRIVATE ${nsh_sources})
    target_include_directories(${TARGET}-lib PUBLIC ${nsh_include_dirs})
    target_compile_features(${TARGET}-lib PUBLIC ${nsh_compile_features})
    # Add compile definitions specific to this lib
    target_compile_definitions(${TARGET}-lib ${ARGN})
    # Add executable with a basic main file
    nsh_add_tool(${TARGET} main.cpp)
    target_link_libraries(${TARGET} PRIVATE ${TARGET}-lib)
    # Append executable binary file to the SIZE_REPORT_LIBS list
    list(APPEND SIZE_REPORT_LIBS $<TARGET_FILE:${TARGET}>)
    set(SIZE_REPORT_LIBS ${SIZE_REPORT_LIBS} PARENT_SCOPE)
endfunction()

nsh_add_size_report_target(nsh_size_report_baseline
    PUBLIC
        NSH_SIZE_REPORT_BASELINE
        NSH_FEATURE_USE_AUTOCOMPLETION=0
        NSH_FEATURE_USE_HISTORY=0
        NSH_FEATURE_USE_PRINTF=0
        NSH_FEATURE_USE_RETURN_CODE_PRINTING=0
)

nsh_add_size_report_target(nsh_size_report_base
    PRIVATE
        NSH_FEATURE_USE_AUTOCOMPLETION=0
        NSH_FEATURE_USE_HISTORY=0
        NSH_FEATURE_USE_PRINTF=0
        NSH_FEATURE_USE_RETURN_CODE_PRINTING=0
)

nsh_add_size_report_target(nsh_size_report_autocomplete
    PRIVATE
        NSH_FEATURE_USE_AUTOCOMPLETION=1
        NSH_FEATURE_USE_HISTORY=0
        NSH_FEATURE_USE_PRINTF=0
        NSH_FEATURE_USE_RETURN_CODE_PRINTING=0
)

nsh_add_size_report_target(nsh_size_report_history
    PRIVATE
        NSH_FEATURE_USE_AUTOCOMPLETION=0
        NSH_FEATURE_USE_HISTORY=1
        NSH_FEATURE_USE_PRINTF=0
        NSH_FEATURE_USE_RETURN_CODE_PRINTING=0
)

nsh_add_size_report_target(nsh_size_report_printf
    PRIVATE
        NSH_FEATURE_USE_AUTOCOMPLETION=0
        NSH_FEATURE_USE_HISTORY=0
        NSH_FEATURE_USE_PRINTF=1
        NSH_FEATURE_USE_RETURN_CODE_PRINTING=0
)

nsh_add_size_report_target(nsh_size_report_return_code_printing
    PRIVATE
        NSH_FEATURE_USE_AUTOCOMPLETION=0
        NSH_FEATURE_USE_HISTORY=0
        NSH_FEATURE_USE_PRINTF=0
        NSH_FEATURE_USE_RETURN_CODE_PRINTING=1
)

nsh_add_size_report_target(nsh_size_report_all_features
    PRIVATE
        NSH_FEATURE_USE_AUTOCOMPLETION=1
        NSH_FEATURE_USE_HISTORY=1
        NSH_FEATURE_USE_PRINTF=1
        NSH_FEATURE_USE_RETURN_CODE_PRINTING=1
)

add_custom_target(nsh-size-report ALL
    COMMAND ${CMAKE_COMMAND} -DBINARY_DIR=${CMAKE_BINARY_DIR} -P PrintCompileOptions.cmake
    COMMAND ${CMAKE_SIZE} --format=berkeley ${SIZE_REPORT_LIBS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json ${SIZE_REPORT_LIBS}
    VERBATIM
)
