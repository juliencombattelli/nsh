/*
 * nsh_cmd.h
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#ifndef NSH_CMD_H_
#define NSH_CMD_H_

#include <nsh/nsh_common_defs.h>
#include <nsh/nsh_config.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int(nsh_cmd_handler_t)(int, char**);

typedef struct nsh_cmd {
    nsh_cmd_handler_t* handler;
    char name[NSH_MAX_STRING_SIZE];
} nsh_cmd_t;

int nsh_cmd_init_empty(nsh_cmd_t* cmd) NON_NULL(1);

int nsh_cmd_init(nsh_cmd_t* cmd, const char* name, nsh_cmd_handler_t* handler) NON_NULL(1, 2);

void nsh_cmd_copy(nsh_cmd_t* dst, const nsh_cmd_t* src) NON_NULL(1, 2);

void nsh_cmd_swap(nsh_cmd_t* cmd1, nsh_cmd_t* cmd2) NON_NULL(1, 2);

#ifdef __cplusplus
}
#endif

#endif /* NSH_CMD_H_ */
