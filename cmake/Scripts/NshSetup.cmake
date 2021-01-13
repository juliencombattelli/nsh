include(${CMAKE_CURRENT_LIST_DIR}/NshCommon.cmake)

if (NOT CMAKE_CROSSCOMPILING)

    nsh_set_platform(Native)

    function(nsh_add_executable)
        add_executable(${ARGV})
    endfunction()

endif()

include(${CMAKE_CURRENT_LIST_DIR}/StaticAnalyzers.cmake)