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

#define xstr(s) str(s)
#define str(s)  #s

nsh_status_t cmd_builtin_version(unsigned int argc, char** argv)
{
#if NSH_FEATURE_USE_PRINTF == 1
    nsh_io_printf("NSH version %u.%u", NSH_VERSION_MAJOR, NSH_VERSION_MINOR);
#else
    nsh_io_put_string("NSH version " xstr(NSH_VERSION_MAJOR) "." xstr(NSH_VERSION_MINOR));
#endif
    return NSH_STATUS_OK;
}
