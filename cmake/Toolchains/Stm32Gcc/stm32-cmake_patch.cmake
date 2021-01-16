set(IN_FILE cmake/stm32/utilities.cmake)

# create list of lines form the contens of a file
file(STRINGS ${IN_FILE} LINES)

# overwrite the file....
file(WRITE ${IN_FILE} "")

# loop through the lines, remove unwanted parts and write the (changed) line ...
foreach(LINE IN LISTS LINES)
    string(REGEX REPLACE "^[ \t]*set[ \t]*\\(FETCHCONTENT_QUIET[ \t]+FALSE\\).*" "" STRIPPED "${LINE}")
    file(APPEND ${IN_FILE} "${STRIPPED}\n")
endforeach()