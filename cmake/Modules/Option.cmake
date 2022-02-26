include(CMakeDependentOption)

function(_nsh_append_option_to_list OPTION_NAME)
    set(NSH_OPTION_LIST "${NSH_OPTION_LIST};${OPTION_NAME}" CACHE INTERNAL "Nsh option list")
endfunction()

function(nsh_clear_option_list)
    set(NSH_OPTION_LIST "" CACHE INTERNAL "Nsh option list")
endfunction()

function(nsh_add_option NAME DESCRIPTION VALUE)
    option(${NAME} ${DESCRIPTION} ${VALUE})
    _nsh_append_option_to_list(${NAME})
endfunction()

function(nsh_add_dependent_option NAME DESCRIPTION VALUE DEPENDS FORCE)
    cmake_dependent_option(${NAME} ${DESCRIPTION} ${VALUE} ${DEPENDS} ${FORCE})
    _nsh_append_option_to_list(${NAME})
endfunction()

function(nsh_print_options)
    message(STATUS "Current user configuration")
    foreach(option IN LISTS NSH_OPTION_LIST)
        if(option)
            message(STATUS "  ${option}=${${option}}")
        endif()
    endforeach()
endfunction()