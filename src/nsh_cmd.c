/*
 * nsh_cmd.c
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#include <nsh/nsh_cmd.h>

#include <string.h>

int nsh_cmd_init_empty(nsh_cmd_t* cmd)
{
    if (!cmd) {
        return NSH_STATUS_WRONG_ARG;
    }
    memset(cmd, 0, sizeof(nsh_cmd_t));
    return NSH_STATUS_OK;
}

int nsh_cmd_init(nsh_cmd_t* cmd, const char* name, nsh_cmd_handler_t* handler)
{
    if (!cmd || !name) {
        return NSH_STATUS_WRONG_ARG;
    }
    size_t name_len = strlen(name);
    if (name_len == 0 || name_len > NSH_MAX_STRING_SIZE) {
        return NSH_STATUS_WRONG_ARG;
    }

    strncpy(cmd->name, name, NSH_MAX_STRING_SIZE);
    cmd->handler = handler;
    return NSH_STATUS_OK;
}

void nsh_cmd_copy(nsh_cmd_t* dst, const nsh_cmd_t* src)
{
    dst->handler = src->handler;
    strncpy(dst->name, src->name, sizeof(dst->name));
}

void nsh_cmd_swap(nsh_cmd_t* cmd1, nsh_cmd_t* cmd2)
{
    nsh_cmd_t temp;
    nsh_cmd_copy(&temp, cmd1);
    nsh_cmd_copy(cmd1, cmd2);
    nsh_cmd_copy(cmd2, &temp);
}

int nsh_cmd_array_init(nsh_cmd_array_t* cmds)
{
    if (!cmds) {
        return NSH_STATUS_WRONG_ARG;
    }

    memset(cmds, 0, sizeof(nsh_cmd_array_t));

    return NSH_STATUS_OK;
}

void nsh_cmd_array_lexicographic_sort(nsh_cmd_array_t* cmds)
{
    for (int i = 0; i < cmds->count - 1; ++i) {
        for (int j = i + 1; j < cmds->count; ++j) {
            if (strcmp(cmds->array[i].name, cmds->array[j].name) > 0) {
                nsh_cmd_swap(&cmds->array[i], &cmds->array[j]);
            }
        }
    }
}

nsh_cmd_t* nsh_cmd_array_find_matching(nsh_cmd_array_t* cmds, const char* partial_name, int name_size)
{
    for (int i = 0; i < cmds->count; i++) {
        if (memcmp(partial_name, cmds->array[i].name, name_size) == 0) {
            return &cmds->array[i];
        }
    }
    return NULL;
}

nsh_cmd_t* nsh_cmd_array_find(nsh_cmd_array_t* cmds, const char* name)
{
    return nsh_cmd_array_find_matching(cmds, name, (int)strlen(name));
}

int nsh_cmd_array_register(nsh_cmd_array_t* cmds, const char* name, nsh_cmd_handler_t* handler)
{
    if (cmds->count >= NSH_CMD_MAX_COUNT) {
        // If we have reached the max cmd count, ignore all registration request
        return NSH_STATUS_MAX_CMD_NB_REACH;
    }

    nsh_cmd_init(&cmds->array[cmds->count], name, handler);

    cmds->count++;

    return NSH_STATUS_OK;
}
