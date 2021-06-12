#ifndef NSH_CMD_HPP_
#define NSH_CMD_HPP_

#include <nsh/nsh_common_defs.hpp>
#include <nsh/nsh_config.hpp>

typedef nsh_status_t nsh_cmd_handler_t(unsigned int, char**);

typedef struct nsh_cmd {
    nsh_cmd_handler_t* handler;
    char name[NSH_MAX_STRING_SIZE];
} nsh_cmd_t;

nsh_status_t nsh_cmd_init_empty(nsh_cmd_t* cmd) NSH_NON_NULL(1);

nsh_status_t nsh_cmd_init(nsh_cmd_t* cmd, const char* name, nsh_cmd_handler_t* handler) NSH_NON_NULL(1, 2);

void nsh_cmd_copy(nsh_cmd_t* dst, const nsh_cmd_t* src) NSH_NON_NULL(1, 2);

void nsh_cmd_swap(nsh_cmd_t* cmd1, nsh_cmd_t* cmd2) NSH_NON_NULL(1, 2);

#endif // NSH_CMD_HPP_
