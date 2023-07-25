#include <nsh/nsh_cmd_builtins.h>

#include <nsh/nsh_io_plugin.h>

nsh_status_t cmd_builtin_help(unsigned int argc, char** argv)
{
    NSH_UNUSED(argc);
    NSH_UNUSED(argv);
    nsh_io_put_string("This is an helpful help message !");
    return NSH_STATUS_OK;
}

nsh_status_t cmd_builtin_exit(unsigned int argc, char** argv)
{
    NSH_UNUSED(argc);
    NSH_UNUSED(argv);
    nsh_io_put_string("exit");
    return NSH_STATUS_QUIT;
}

nsh_status_t cmd_builtin_version(unsigned int argc, char** argv)
{
    NSH_UNUSED(argc);
    NSH_UNUSED(argv);
#if NSH_FEATURE_USE_PRINTF == 1
    nsh_io_printf("Nsh version %u.%u.%u", NSH_VERSION_MAJOR, NSH_VERSION_MINOR, NSH_VERSION_PATCH);
#else
    nsh_io_put_string("Nsh version " NSH_VERSION_STRING);
#endif
    return NSH_STATUS_OK;
}
