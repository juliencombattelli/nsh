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

typedef struct nsh_cmd_array {
    nsh_cmd_t array[NSH_CMD_MAX_COUNT];
    int count;
} nsh_cmd_array_t;

int nsh_cmd_init_empty(nsh_cmd_t* cmd);

int nsh_cmd_init(nsh_cmd_t* cmd, const char* name, nsh_cmd_handler_t* handler);

void nsh_cmd_copy(nsh_cmd_t* dst, const nsh_cmd_t* src);

void nsh_cmd_swap(nsh_cmd_t* cmd1, nsh_cmd_t* cmd2);

int nsh_cmd_array_init(nsh_cmd_array_t* cmds);

void nsh_cmd_array_lexicographic_sort(nsh_cmd_array_t* cmds);

nsh_cmd_t* nsh_cmd_array_find_matching(nsh_cmd_array_t* cmds, const char* partial_name, int name_size);

nsh_cmd_t* nsh_cmd_array_find(nsh_cmd_array_t* cmds, const char* name);

int nsh_cmd_array_register(nsh_cmd_array_t* cmds, const char* name, nsh_cmd_handler_t* handler);

#ifdef __cplusplus
}
#endif

#endif /* NSH_CMD_H_ */
