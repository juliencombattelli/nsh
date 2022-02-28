nsh_add_library(nshpp
    src/Stdio.cpp
)
target_include_directories(nshpp
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)
target_compile_features(nshpp
    PUBLIC
        cxx_std_20
)

add_library(Nsh::Nshpp ALIAS nshpp)

# TODO configure nshpp installation
# install(
#     TARGETS nsh
#     RUNTIME DESTINATION bin
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
# )

if(NSH_ENABLE_TESTS)
    add_subdirectory(test)
endif()
