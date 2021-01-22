# Distributed under the MIT License. See accompanying LICENSE file for details.

#[=======================================================================[.rst:
FindGDB
-----------

This module checks if GDB is installed and determines where the executable is.
The caller may set ``GDB_HOME`` to specify a GDB installation prefix explicitly.

The module defines the following ``IMPORTED`` targets:

``GDB::GDB``
  Executable of the GDB debugger

The module defines the following variables:

``GDB_FOUND``
  Was the GDB executable found
``GDB_EXECUTABLE``
  Path to the GDB executable
``GDB_VERSION_STRING``
  GDB version found e.g. 2.5
``GDB_VERSION_MAJOR``
  GDB major version found e.g. 2
``GDB_VERSION_MINOR``
  GDB minor version found e.g. 5

Example usage:

.. code-block:: cmake

  find_package(GDB COMPONENTS armv7e-m i386)
  if(GDB_FOUND)
    message("GDB found: ${GDB_EXECUTABLE}")
  endif()

#]=======================================================================]

set(_gdb_names "")

# If the current toolchain defines a compiler target prefix, use it
if(DEFINED CMAKE_C_COMPILER_TARGET)
    list(APPEND _gdb_names ${CMAKE_C_COMPILER_TARGET}-gdb)
endif()

list(APPEND _gdb_names gdb-multiarch gdb)

# Add user-provided GDB installation directory to search paths
set(_gdb_paths)
if(GDB_HOME)
    list(APPEND _gdb_paths ${GDB_HOME}/bin)
endif()

# If running on Windows, search for *.exe as well
if(CMAKE_HOST_WIN32)
    list(APPEND _gdb_names ${_gdb_names}.exe)
endif()

# Search the PATH and specific locations
find_program(GDB_EXECUTABLE
    NAMES ${_gdb_names}
    PATHS ${_gdb_paths}
    DOC "Path to the GDB executable"
)

unset(_gdb_names)
unset(_gdb_paths)

if(GDB_EXECUTABLE)
    # Get GDB version message
    execute_process(
        COMMAND ${GDB_EXECUTABLE} --version
        OUTPUT_VARIABLE _gdb_version
        RESULT_VARIABLE _gdb_version_result
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # If GDB version command returned successfully
    if(NOT _gdb_version_result)
        # Extract version string
        string(REGEX REPLACE "[^0-9]*([0-9]+[0-9.]*).*" "\\1" GDB_VERSION_STRING "${_gdb_version}")
        string(REPLACE "." ";" _gdb_version_list "${GDB_VERSION_STRING}")
        list(GET _gdb_version_list 0 GDB_VERSION_MAJOR)
        list(GET _gdb_version_list 1 GDB_VERSION_MINOR)
    endif()

    unset(_gdb_version)
    unset(_gdb_version_list)

    foreach(_arch IN LISTS GDB_FIND_COMPONENTS)
        # Check if this architecture is supported
        execute_process(
            COMMAND ${GDB_EXECUTABLE} --batch -ex "set architecture ${_arch}"
            RESULT_VARIABLE _check_result
            OUTPUT_QUIET ERROR_QUIET
        )
        # If arch is supported, set the corresponding component variable
        if(NOT _check_result EQUAL 1)
            set(GDB_${_arch}_FOUND TRUE)
        endif()
        unset(_check_result)
    endforeach()
endif()

# Process find_package arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    GDB
    REQUIRED_VARS
        GDB_EXECUTABLE 
    VERSION_VAR
        GDB_VERSION_STRING
    HANDLE_COMPONENTS
)

if(GDB_FOUND)
    mark_as_advanced(GDB_EXECUTABLE)
    # Export GDB executable target
    if(GDB_IS_VALID AND NOT TARGET GDB::GDB)
        add_executable(GDB::GDB IMPORTED)
        set_property(TARGET GDB::GDB PROPERTY IMPORTED_LOCATION "${GDB_EXECUTABLE}")
    endif()
endif()