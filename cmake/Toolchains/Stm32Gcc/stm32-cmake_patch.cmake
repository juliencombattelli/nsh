# Distributed under the MIT License. See accompanying LICENSE file for details.

#[=======================================================================[.rst:
stm32-cmake-patch
-----------------

Patch stm32-cmake repository.
Here is the list of the patches applied with the rationale behing them:

``Remove FETCHCONTENT_QUIET set to FALSE``
  Remove all occurences of set(FETCHCONTENT_QUIET FALSE) in cmake/stm32/utilities.cmake.
  FETCHCONTENT_QUIET is a cache variable set to ON by default, preventing FetchContent population
  to be too noisy. This variable should be only set by the user if wanted, and never hardcoded
  in scripts.

``find_program in REQUIRED mode for compilers``
  NOTE: Directly implemented in Stm32Gcc.cmake. Just mentionned in comment for complexe regex example.
  If the find_program is not done in REQUIRED mode and if the requested compiler is not found, CMake
  will always fallback to system compiler on Linux (eg. /usr/bin/cc), even if it is not compatible
  with the targeted platform.

#]=======================================================================]

################################################################################
### Remove set(FETCHCONTENT_QUIET FALSE) from cmake/stm32/utilities.cmake

set(IN_FILE cmake/stm32/utilities.cmake)

# create list of lines form the contens of a file
file(STRINGS ${IN_FILE} LINES)

# overwrite the file....
file(WRITE ${IN_FILE} "")

# loop through the lines, remove unwanted parts and write the (changed) line ...
foreach(LINE IN LISTS LINES)
    string(REGEX REPLACE "^[ \t]*set[ \t]*\\([ \t]*FETCHCONTENT_QUIET[ \t]+FALSE[ \t]*\\).*" "" STRIPPED "${LINE}")
    file(APPEND ${IN_FILE} "${STRIPPED}\n")
endforeach()

################################################################################
### Find compilers in REQUIRED mode

# set(IN_FILE cmake/stm32_gcc.cmake)

# # create list of lines form the contens of a file
# file(STRINGS ${IN_FILE} LINES)

# # overwrite the file....
# file(WRITE ${IN_FILE} "")

# # loop through the lines, append REQUIRED for each compiler find_program and write the (changed) line ...
# foreach(LINE IN LISTS LINES)
#     string(REGEX REPLACE
#         "^([ \t]*find_program[ \t]*\\([ \t]*CMAKE_[CXASM]+_COMPILER[ \t]+NAMES[ \t]+\\\${STM32_TARGET_TRIPLET}-[gc+]+[ \t]+PATHS[ \t]+\\\${TOOLCHAIN_BIN_PATH})([ \t]*\\).*)"
#         "\\1 REQUIRED\\2"
#         STRIPPED "${LINE}"
#     )
#     file(APPEND ${IN_FILE} "${STRIPPED}\n")
# endforeach()