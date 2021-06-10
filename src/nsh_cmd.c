#include <nsh/nsh_cmd.h>

#include <string.h>

int nsh_cmd_init_empty(nsh_cmd_t* cmd)
{
    memset(cmd, 0, sizeof(nsh_cmd_t));
    return NSH_STATUS_OK;
}

int nsh_cmd_init(nsh_cmd_t* cmd, const char* name, nsh_cmd_handler_t* handler)
{
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
