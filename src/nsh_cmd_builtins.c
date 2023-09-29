#include <nsh/nsh_cmd_builtins.h>
#include <nsh/nsh_io_plugin_default.h>

// TODO move to an appropriate place to avoid duplicates
#if NSH_IO_PLUGIN_IS_STATIC == 1
#define nsh_io_plugin(nsh) ((void)nsh, NSH_IO_PLUGIN_STATIC)
#else
#define nsh_io_plugin(nsh) (((const nsh_t*)nsh)->io)
#endif

nsh_status_t cmd_builtin_help(const nsh_t* nsh, unsigned int argc, char** argv)
{
    NSH_UNUSED(argc);
    NSH_UNUSED(argv);
    // nsh_io_put_string("This is an helpful help message !");
    nsh_io_plugin(nsh).put_string("This is an helpful help message !");
    return NSH_STATUS_OK;
}

nsh_status_t cmd_builtin_exit(const nsh_t* nsh, unsigned int argc, char** argv)
{
    NSH_UNUSED(argc);
    NSH_UNUSED(argv);
    NSH_UNUSED(nsh);
    // nsh_io_put_string("exit");
    return NSH_STATUS_QUIT;
}

nsh_status_t cmd_builtin_version(const nsh_t* nsh, unsigned int argc, char** argv)
{
    NSH_UNUSED(argc);
    NSH_UNUSED(argv);
    NSH_UNUSED(nsh);
#if NSH_FEATURE_USE_PRINTF == 1
    // nsh_io_printf("Nsh version %u.%u.%u", NSH_VERSION_MAJOR, NSH_VERSION_MINOR, NSH_VERSION_PATCH);
#else
    // nsh_io_put_string("Nsh version " NSH_VERSION_STRING);
#endif
    return NSH_STATUS_OK;
}
