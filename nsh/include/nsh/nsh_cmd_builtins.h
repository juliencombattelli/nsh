#ifndef NSH_CMD_BUILTINS_HPP_
#define NSH_CMD_BUILTINS_HPP_

#include <nsh/nsh_common_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

nsh_status_t cmd_builtin_help(unsigned int argc, char** argv);

nsh_status_t cmd_builtin_exit(unsigned int argc, char** argv);

#ifdef __cplusplus
}
#endif

#endif // NSH_CMD_BUILTINS_HPP_
