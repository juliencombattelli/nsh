/*
 * shell_cmd_builtins.c
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#include "shell_cmd_builtins.h"
#include "shell_common_defs.h"
#include <stdio.h>

int cmd_builtin_help(int argc, char **argv)
{
    SHELL_UNUSED(argc);
    SHELL_UNUSED(argv);
    puts("This is an helpful help message !");
    return SHELL_STATUS_FAILURE;
}

