/*
 * nsh_cmd_builtins.c
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#include <nsh/nsh_cmd_builtins.h>
#include <nsh/nsh_common_defs.h>

#include <stdio.h>

int cmd_builtin_help(int argc, char** argv)
{
    NSH_UNUSED(argc);
    NSH_UNUSED(argv);
    puts("This is an helpful help message !");
    return NSH_STATUS_OK;
}

int cmd_builtin_exit(int argc, char** argv)
{
    NSH_UNUSED(argc);
    NSH_UNUSED(argv);
    puts("exit");
    return NSH_STATUS_QUIT;
}
