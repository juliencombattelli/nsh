/*
 * shell_cmd.h
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#ifndef SHELL_CMD_H_
#define SHELL_CMD_H_

#include "shell_common_defs.h"
#include "shell_config.h"

typedef char shell_cmd_name_t[SHELL_MAX_STRING_SIZE];
typedef int (*shell_cmd_handler_t)(int, char**);

typedef struct shell_cmd
{
    shell_cmd_handler_t handler;
    shell_cmd_name_t name;

} shell_cmd_t;

typedef struct shell_cmd_array
{
	shell_cmd_t array[SHELL_CMD_MAX_COUNT];
	int count;

} shell_cmd_array_t;

void shell_cmd_copy(shell_cmd_t *dst, const shell_cmd_t *src);
void shell_cmd_swap(shell_cmd_t *cmd1, shell_cmd_t *cmd2);

void shell_cmd_lexicographic_sort(shell_cmd_array_t *cmds);
shell_cmd_t* shell_cmd_find_matching(shell_cmd_array_t *cmds, const char *partial_name, int name_size);
shell_cmd_t* shell_cmd_find(shell_cmd_array_t *cmds, const char *name);
int shell_cmd_register(shell_cmd_array_t *cmds, const char *name, shell_cmd_handler_t handler);

#endif /* SHELL_CMD_H_ */
