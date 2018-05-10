/*
 * shell_cmd.h
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#ifndef SHELL_CMD_H_
#define SHELL_CMD_H_

#include "shell_config.h"

typedef char shell_cmd_name_t[SHELL_CMD_NAME_SIZE];
typedef int (*shell_cmd_handler_t)(int, char**);

typedef struct shell_cmd
{
    shell_cmd_handler_t handler;
    shell_cmd_name_t name;

} shell_cmd_t;

void shell_cmdcpy(shell_cmd_t *dst, const shell_cmd_t *src);
void shell_cmdswap(shell_cmd_t *cmd1, shell_cmd_t *cmd2);

#endif /* SHELL_CMD_H_ */
