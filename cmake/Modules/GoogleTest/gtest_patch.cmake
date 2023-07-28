# Distributed under the MIT License. See accompanying LICENSE file for details.

#[=======================================================================[.rst:
gtest-patch
-----------------

Patch googletest repository.
Here is the list of the patches applied with the rationale behing them:

``Use #if to check filesystem support``
  Replace #ifdef with #if to check GTEST_HAS_FILE_SYSTEM since the flag is always defined and #ifdef
  does not work as intended here.

#]=======================================================================]

set(IN_FILE googletest/src/gtest-internal-inl.h)

# create list of lines form the contens of a file
file(STRINGS ${IN_FILE} LINES)

# overwrite the file....
file(WRITE ${IN_FILE} "")

# Known limitation with bracket handling in CMake lists
# https://gitlab.kitware.com/cmake/cmake/-/issues/19156
# Replace square brackets with arbitrary tokens to avoid the issue above
string(REGEX REPLACE "\\[" "_NSH_BRACKET_OPEN" LINES "${LINES}")
string(REGEX REPLACE "\\]" "_NSH_BRACKET_CLOSE" LINES "${LINES}")

# loop through the lines, remove unwanted parts and write the (changed) line ...
foreach(LINE IN LISTS LINES)
    # Replace back the tokens with the brackets
    string(REGEX REPLACE "_NSH_BRACKET_OPEN" "[" LINE "${LINE}")
    string(REGEX REPLACE "_NSH_BRACKET_CLOSE" "]" LINE "${LINE}")

    string(REPLACE
        "#ifdef GTEST_HAS_FILE_SYSTEM"
        "#if GTEST_HAS_FILE_SYSTEM"
        LINE "${LINE}")

    file(APPEND ${IN_FILE} "${LINE}\n")
endforeach()
