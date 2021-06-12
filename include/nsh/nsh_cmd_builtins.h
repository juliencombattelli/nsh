#ifndef NSH_CMD_BUILTINS_H_
#define NSH_CMD_BUILTINS_H_

#include <nsh/nsh_common_defs.h>

nsh_status_t cmd_builtin_help(unsigned int argc, char** argv);

nsh_status_t cmd_builtin_exit(unsigned int argc, char** argv);

#endif // NSH_CMD_BUILTINS_H_
