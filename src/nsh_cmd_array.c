#include <nsh/nsh_cmd_array.h>

#include <string.h>

int nsh_cmd_array_init(nsh_cmd_array_t* cmds)
{
    memset(cmds, 0, sizeof(nsh_cmd_array_t));
    return NSH_STATUS_OK;
}

int nsh_cmd_array_lexicographic_sort(nsh_cmd_array_t* cmds)
{
    for (unsigned int i = 0; i < cmds->count - 1; ++i) {
        for (unsigned int j = i + 1; j < cmds->count; ++j) {
            if (strcmp(cmds->array[i].name, cmds->array[j].name) > 0) {
                nsh_cmd_swap(&cmds->array[i], &cmds->array[j]);
            }
        }
    }
    return NSH_STATUS_OK;
}

const nsh_cmd_t* nsh_cmd_array_find_matching(const nsh_cmd_array_t* cmds, const char* partial_name, unsigned int name_size)
{
    for (unsigned int i = 0; i < cmds->count; i++) {
        if (memcmp(partial_name, cmds->array[i].name, name_size) == 0) {
            return &cmds->array[i];
        }
    }
    return NULL;
}

const nsh_cmd_t* nsh_cmd_array_find(const nsh_cmd_array_t* cmds, const char* name)
{
    return nsh_cmd_array_find_matching(cmds, name, (unsigned int)strlen(name));
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
