#ifndef NSH_LINE_BUFFER_H_
#define NSH_LINE_BUFFER_H_

#include <nsh/nsh_common_defs.h>
#include <nsh/nsh_config.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nsh_line_buffer {
    char buffer[NSH_LINE_BUFFER_SIZE];
    unsigned int size;
} nsh_line_buffer_t;

void nsh_line_buffer_reset(nsh_line_buffer_t* linebuf) NSH_NON_NULL(1);

nsh_status_t nsh_line_buffer_append_char(nsh_line_buffer_t* linebuf, char c) NSH_NON_NULL(1);

void nsh_line_buffer_append_null(nsh_line_buffer_t* linebuf) NSH_NON_NULL(1);

void nsh_line_buffer_erase_last_char(nsh_line_buffer_t* linebuf) NSH_NON_NULL(1);

bool nsh_line_buffer_is_full(nsh_line_buffer_t* linebuf) NSH_NON_NULL(1);

bool nsh_line_buffer_is_empty(nsh_line_buffer_t* linebuf) NSH_NON_NULL(1);

#ifdef __cplusplus
}
#endif

#endif // NSH_LINE_BUFFER_H_
