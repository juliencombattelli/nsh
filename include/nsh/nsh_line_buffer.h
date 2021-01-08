#ifndef NSH_LINE_BUFFER_H_
#define NSH_LINE_BUFFER_H_

#include <nsh/nsh_common_defs.h>
#include <nsh/nsh_config.h>

#include <stdbool.h>

typedef struct nsh_line_buffer {
    char buffer[NSH_LINE_BUFFER_SIZE];
    int size;
} nsh_line_buffer_t;

void nsh_line_buffer_reset(nsh_line_buffer_t* linebuf);

int nsh_line_buffer_append_char(nsh_line_buffer_t* linebuf, char c);

void nsh_line_buffer_append_null(nsh_line_buffer_t* linebuf);

void nsh_line_buffer_erase_last_char(nsh_line_buffer_t* linebuf);

bool nsh_line_buffer_is_full(nsh_line_buffer_t* linebuf);

bool nsh_line_buffer_is_empty(nsh_line_buffer_t* linebuf);

#endif /* NSH_LINE_BUFFER_H_ */
