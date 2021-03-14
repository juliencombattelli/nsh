#ifndef NSH_CMD_ARRAY_H_
#define NSH_CMD_ARRAY_H_

#include <nsh/nsh_cmd.h>
#include <nsh/nsh_common_defs.h>
#include <nsh/nsh_config.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nsh_cmd_array {
    nsh_cmd_t array[NSH_CMD_MAX_COUNT];
    unsigned int count;
} nsh_cmd_array_t;

int nsh_cmd_array_init(nsh_cmd_array_t* cmds) NON_NULL(1);

int nsh_cmd_array_lexicographic_sort(nsh_cmd_array_t* cmds) NON_NULL(1);

nsh_cmd_t* nsh_cmd_array_find_matching(nsh_cmd_array_t* cmds, const char* partial_name, unsigned int name_size) NON_NULL(1, 2);

nsh_cmd_t* nsh_cmd_array_find(nsh_cmd_array_t* cmds, const char* name) NON_NULL(1, 2);

int nsh_cmd_array_register(nsh_cmd_array_t* cmds, const char* name, nsh_cmd_handler_t* handler) NON_NULL(1, 2);

#ifdef __cplusplus
}
#endif

#endif /* NSH_CMD_ARRAY_H_ */
