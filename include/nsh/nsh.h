#ifndef NSH_H_
#define NSH_H_

#include <nsh/nsh_cmd.h>
#include <nsh/nsh_cmd_array.h>
#include <nsh/nsh_config.h>
#include <nsh/nsh_history.h>
#include <nsh/nsh_io_plugin.h>
#include <nsh/nsh_line_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nsh_s {
    nsh_io_plugin_t io;
    nsh_line_buffer_t line;
    nsh_cmd_array_t cmds;
#if NSH_FEATURE_USE_HISTORY == 1
    nsh_history_t history;
    unsigned int current_history_entry;
#endif
} nsh_t;

nsh_t nsh_init(nsh_io_plugin_t io, nsh_status_t* status) NSH_NON_NULL(2);

nsh_status_t nsh_register_command(nsh_t* nsh, const char* name, nsh_cmd_handler_t* handler);

void nsh_run(nsh_t* nsh);

#ifdef __cplusplus
}
#endif

#endif // NSH_H_
