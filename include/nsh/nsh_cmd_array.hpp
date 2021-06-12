#ifndef NSH_CMD_ARRAY_HPP_
#define NSH_CMD_ARRAY_HPP_

#include <nsh/nsh_cmd.hpp>
#include <nsh/nsh_common_defs.hpp>
#include <nsh/nsh_config.hpp>

typedef struct nsh_cmd_array {
    nsh_cmd_t array[NSH_CMD_MAX_COUNT];
    unsigned int count;
} nsh_cmd_array_t;

nsh_status_t nsh_cmd_array_init(nsh_cmd_array_t* cmds)
    NSH_NON_NULL(1);

nsh_status_t nsh_cmd_array_lexicographic_sort(nsh_cmd_array_t* cmds)
    NSH_NON_NULL(1);

const nsh_cmd_t* nsh_cmd_array_find_matching(const nsh_cmd_array_t* cmds, const char* partial_name, unsigned int name_size)
    NSH_NON_NULL(1, 2);

const nsh_cmd_t* nsh_cmd_array_find(const nsh_cmd_array_t* cmds, const char* name)
    NSH_NON_NULL(1, 2);

nsh_status_t nsh_cmd_array_register(nsh_cmd_array_t* cmds, const char* name, nsh_cmd_handler_t* handler)
    NSH_NON_NULL(1, 2);

#endif // NSH_CMD_ARRAY_HPP_
