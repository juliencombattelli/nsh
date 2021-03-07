function(_nsh_assert_valid_build_type_for_coverage)
    set(_supported_coverage_build_types Debug)
    if(NOT CMAKE_BUILD_TYPE IN_LIST _supported_coverage_build_types)
        message(WARNING
            " CMAKE_BUILD_TYPE not compatible with test coverage.\n"
            " Current: ${CMAKE_BUILD_TYPE},  Supported: ${_supported_coverage_build_types}.\n"
            " Coverage will be disabled."
        )
        set(ENABLE_COVERAGE OFF CACHE BOOL "" FORCE)
    endif()
    unset(_supported_coverage_build_types)
endfunction()

if(ENABLE_COVERAGE)
    _nsh_assert_valid_build_type_for_coverage()

    target_compile_options(nsh
        PRIVATE
            "$<$<C_COMPILER_ID:GNU>:--coverage>"
    )
    # target_link_options does not work to add coverage link flag because nsh is a static lib...
    # target_link_options(nsh
    #     PRIVATE
    #         "$<$<C_COMPILER_ID:GNU>:--coverage>"
    # )
    # ...so use target_link_libraries instead
    target_link_libraries(nsh
        PRIVATE
            "$<$<C_COMPILER_ID:GNU>:--coverage>"
    )

    # TODO add FindLCOV find module
    # Checkout https://github.com/RWTH-HPC/CMake-codecov for some design ideas
    add_custom_target(coverage
        COMMAND lcov --capture --directory . --output-file coverage.info
        COMMAND lcov --directory . --output-file coverage.info --remove coverage.info "/usr/" ".moc" "test/*"
        COMMAND genhtml -o coverage coverage.info
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

endif()

