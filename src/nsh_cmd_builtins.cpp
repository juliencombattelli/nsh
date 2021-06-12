#include <nsh/nsh_cmd_builtins.hpp>

#include <stdio.h>

nsh_status_t cmd_builtin_help(unsigned int argc, char** argv)
{
    NSH_UNUSED(argc);
    NSH_UNUSED(argv);
    puts("This is an helpful help message !");
    return NSH_STATUS_OK;
}

nsh_status_t cmd_builtin_exit(unsigned int argc, char** argv)
{
    NSH_UNUSED(argc);
    NSH_UNUSED(argv);
    puts("exit");
    return NSH_STATUS_QUIT;
}
