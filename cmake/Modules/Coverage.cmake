function(_nsh_assert_valid_build_type_for_coverage)
    set(_supported_coverage_build_types Debug)
    if(NOT CMAKE_BUILD_TYPE IN_LIST _supported_coverage_build_types)
        message(FATAL_ERROR "CMAKE_BUILD_TYPE not compatible with test coverage. (Current: ${CMAKE_BUILD_TYPE},  Supported: ${_supported_coverage_build_types})")
    endif()
    unset(_supported_coverage_build_types)
endfunction()

if(ENABLE_COVERAGE)
    _nsh_assert_valid_build_type_for_coverage()

    # add --coverage option to Nsh::Nsh if gnu compiler

    # define custom target starting coverage like in https://www.labri.fr/perso/allali/?page_id=1146

endif()

