/*
 * nsh_cmd_builtins.h
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#ifndef NSH_CMD_BUILTINS_H_
#define NSH_CMD_BUILTINS_H_

#ifdef __cplusplus
extern "C" {
#endif

int cmd_builtin_help(unsigned int argc, char** argv);

int cmd_builtin_exit(unsigned int argc, char** argv);

#ifdef __cplusplus
}
#endif

#endif /* NSH_CMD_BUILTINS_H_ */
