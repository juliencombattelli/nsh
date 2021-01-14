# Distributed under the MIT License. See accompanying LICENSE file for details.

#[=======================================================================[.rst:
FindOpenOCD
----------------

This module finds if OpenOCD debugger is installed and determines where the
executables are. The caller may set ``OPENOCD_HOME`` to specify a OpenOCD
installation prefix explicitly.

The module defines the following ``IMPORTED`` targets:

``OpenOCD::OpenOCD``
  Executable of the OpenOCD debugger
  
The module defines the following variables:

``OPENOCD_FOUND``
  Was the OpenOCD executable found
``OPENOCD_EXECUTABLE``
  Path to the OpenOCD executable
``OPENOCD_VERSION_STRING``
  OpenOCD version found e.g. 2.5.7
``OPENOCD_VERSION_MAJOR``
  OpenOCD major version found e.g. 2
``OPENOCD_VERSION_MINOR``
  OpenOCD minor version found e.g. 5
``OPENOCD_VERSION_PATCH``
  OpenOCD patch version found e.g. 7

Example usage:

.. code-block:: cmake

  find_package(OpenOCD)
  if(OPENOCD_FOUND)
    message("OpenOCD found: ${OPENOCD_EXECUTABLE}")
  endif()

#]=======================================================================]

set(_openocd_names openocd)

# If running on Windows, search for openocd.exe as well
if(CMAKE_HOST_WIN32)
  list(APPEND _openocd_names openocd.exe)
endif()

# Add user-provided OpenOCD installation directory to search paths
set(_openocd_paths)
if(OPENOCD_HOME)
  list(APPEND _openocd_paths ${OPENOCD_HOME}/bin)
endif()

# First search the PATH and specific locations.
find_program(OPENOCD_EXECUTABLE
  NAMES ${_openocd_names}
  PATHS ${_openocd_paths}
  DOC "Path to the OpenOCD executable"
)

unset(_openocd_names)
unset(_openocd_paths)

if(OPENOCD_EXECUTABLE)
  # Determine OpenOCD version string
  execute_process(
    COMMAND "${OPENOCD_EXECUTABLE}" --version
    ERROR_VARIABLE _openocd_version # OpenOCD outputs version into stderr
    RESULT_VARIABLE _openocd_version_result
  )
  if(NOT _openocd_version_result)
    string(REGEX MATCH "^Open On-Chip Debugger ([0-9]+.[0-9]+.[0-9]+) " _openocd_version_match "${_openocd_version}")
    set(OPENOCD_VERSION_STRING ${CMAKE_MATCH_1})
    string(REPLACE "." ";" _openocd_version_list "${OPENOCD_VERSION_STRING}")
    list(GET _openocd_version_list 0 OPENOCD_VERSION_MAJOR)
    list(GET _openocd_version_list 1 OPENOCD_VERSION_MINOR)
    list(GET _openocd_version_list 2 OPENOCD_VERSION_PATCH)
    unset(_openocd_version_match)
    unset(_openocd_version_list)
  endif()
  unset(_openocd_version)
  unset(_openocd_version_result)

  # Export OpenOCD target
  if(NOT TARGET OpenOCD::OpenOCD)
    add_executable(OpenOCD::OpenOCD IMPORTED)
    set_property(TARGET OpenOCD::OpenOCD PROPERTY IMPORTED_LOCATION "${OPENOCD_EXECUTABLE}")
  endif()
endif()

# Process find_package arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenOCD
  REQUIRED_VARS OPENOCD_EXECUTABLE VERSION_VAR OPENOCD_VERSION_STRING
)

mark_as_advanced(OPENOCD_EXECUTABLE)