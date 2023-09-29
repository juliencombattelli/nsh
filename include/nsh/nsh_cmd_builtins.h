#ifndef NSH_CMD_BUILTINS_H_
#define NSH_CMD_BUILTINS_H_

#include <nsh/nsh_cmd.h>
#include <nsh/nsh_common_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

nsh_status_t cmd_builtin_help(const nsh_t* nsh, unsigned int argc, char** argv);

nsh_status_t cmd_builtin_exit(const nsh_t* nsh, unsigned int argc, char** argv);

nsh_status_t cmd_builtin_version(const nsh_t* nsh, unsigned int argc, char** argv);

#ifdef __cplusplus
}
#endif

#endif // NSH_CMD_BUILTINS_H_
