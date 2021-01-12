set(_nsh_NSH_NATIVE_doc "If set, the project is configured to be built on native platform. If false, the project is configured to be cross-compiled.")
if (NOT CMAKE_CROSSCOMPILING)
    set(NSH_NATIVE TRUE CACHE INTERNAL _nsh_NSH_NATIVE_doc)
    include (${CMAKE_CURRENT_LIST_DIR}/../Platforms/Native.cmake)
else()
    set(NSH_NATIVE FALSE CACHE INTERNAL _nsh_NSH_NATIVE_doc)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/StaticAnalyzers.cmake)