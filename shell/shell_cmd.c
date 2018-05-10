/*
 * shell_cmd.c
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#include "shell_cmd.h"
#include <string.h>

void shell_cmdcpy(shell_cmd_t *dst, const shell_cmd_t *src)
{
    dst->handler = src->handler;
    strcpy(dst->name, src->name);
}

void shell_cmdswap(shell_cmd_t *cmd1, shell_cmd_t *cmd2)
{
    shell_cmd_t temp;
    shell_cmdcpy(&temp, cmd1);
    shell_cmdcpy(cmd1, cmd2);
    shell_cmdcpy(cmd2, &temp);
}

