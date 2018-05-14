/*
 * shell_cmd.c
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#include "shell_cmd.h"
#include <string.h>

void shell_cmd_copy(shell_cmd_t *dst, const shell_cmd_t *src)
{
    dst->handler = src->handler;
    strcpy(dst->name, src->name);
}

void shell_cmd_swap(shell_cmd_t *cmd1, shell_cmd_t *cmd2)
{
    shell_cmd_t temp;
    shell_cmd_copy(&temp, cmd1);
    shell_cmd_copy(cmd1, cmd2);
    shell_cmd_copy(cmd2, &temp);
}

void shell_cmd_lexicographic_sort(shell_cmd_array_t *cmds)
{
   for (int i = 0; i < cmds->count - 1; ++i)
       for (int j = i + 1; j < cmds->count; ++j)
           if (strcmp(cmds->array[i].name, cmds->array[j].name) > 0)
               shell_cmd_swap(&cmds->array[i], &cmds->array[j]);
}

shell_cmd_t* shell_cmd_find_matching(shell_cmd_array_t *cmds, const char *partial_name, int name_size)
{
    for (int i = 0; i < cmds->count; i++)
        if (memcmp(partial_name, cmds->array[i].name, name_size) == 0)
            return &cmds->array[i];
    return NULL;
}

shell_cmd_t* shell_cmd_find(shell_cmd_array_t *cmds, const char *name)
{
    return shell_cmd_find_matching(cmds, name, strlen(name));
}

int shell_cmd_register(shell_cmd_array_t *cmds, const char *name, shell_cmd_handler_t handler)
{
    if (cmds->count >= SHELL_CMD_MAX_COUNT)
        // If we have reached the max cmd count, ignore all registration request
        return SHELL_STATUS_MAX_CMD_NB_REACH;

    size_t name_len = strlen(name);
    if (name_len == 0 || name_len > SHELL_MAX_STRING_SIZE || !handler)
        return SHELL_STATUS_WRONG_ARG;

    strcpy(cmds->array[cmds->count].name, name);
    cmds->array[cmds->count].handler = handler;

    cmds->count++;

    return SHELL_STATUS_OK;
}
