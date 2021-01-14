set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../Modules ${CMAKE_MODULE_PATH})

function(nsh_set_platform PLATFORM)
    set(NSH_PLATFORM ${PLATFORM} CACHE INTERNAL "Current targeted platform.")
endfunction()