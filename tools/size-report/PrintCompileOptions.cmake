# Distributed under the MIT License. See accompanying LICENSE file for details.

#[=======================================================================[.rst:
PrintCompileOptions
-------------------

Parse a cmake-generated compile_commands.json compilation database and print the compile
options of the main.cpp source file.

#]=======================================================================]

# string(JSON) was introduced in 3.19
cmake_minimum_required(VERSION 3.19)

# Get the absolute path of main.c file
file(REAL_PATH main.cpp report_size_main)

# TODO Check if compile_commands exists
if(NOT EXISTS ${BINARY_DIR}/compile_commands.json)
    message(FATAL_ERROR
        " ${BINARY_DIR}/compile_commands.json does not exist.\n"
        " Ensure the project has been configured and CMAKE_EXPORT_COMPILE_COMMANDS enabled."
    )
endif()

# Read the compile_commands.json file
file(READ ${BINARY_DIR}/compile_commands.json compile_commands_json)

# Get the number of elements
string(JSON len LENGTH "${compile_commands_json}")

# Iterate over the elements in compile_commands.json
math(EXPR len "${len} - 1")
foreach(index RANGE ${len})
    # Get the index-th element of the compile_commands array
    string(JSON element GET "${compile_commands_json}" ${index})
    # Get the filename of the current element
    string(JSON filename GET "${element}" file)
    if(${filename} STREQUAL ${report_size_main})
        # If the filename matches the main file, get the command line used to build it
        string(JSON cmdline GET "${element}" command)
        break()
    endif()
endforeach()

# Remove the first word which should be the compiler executable
string(REGEX REPLACE "^[a-zA-Z0-9_/.\"\\+-]+(.*)$" "\\1" cmdline ${cmdline})

# Remove spaces between one-letter options and values
string(REGEX REPLACE "[ \t\r\n](-[Box])[ \t\r\n]+" " \\1" cmdline ${cmdline})

# Remove spaces between -isystem options and values
string(REGEX REPLACE "[ \t\r\n](-isystem)[ \t\r\n]+" " \\1" cmdline ${cmdline})

# Remove any parameter not starting with - which should be files to build
string(REGEX REPLACE "[ \t\r\n]+[^-][a-zA-Z0-9_/.\"\\=+-]+" "" cmdline ${cmdline})

# Remove --sysroot option
string(REGEX REPLACE "[ \t\r\n]--sysroot(=|[ \t\r\n]+)[a-zA-Z0-9_/.\"\\+-]+" "" cmdline ${cmdline})

# Remove -W, -I, -i, -L, -l, -o, -c, -g options
string(REGEX REPLACE "[ \t\r\n]-[WIiLlocg][a-zA-Z0-9_./\\=+-]*" "" cmdline ${cmdline})

# Print result
message("\n\nCompile flags: ${cmdline}\n")
