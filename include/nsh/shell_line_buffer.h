#ifndef SHELL_LINE_BUFFER_H_
#define SHELL_LINE_BUFFER_H_

#include <nsh/shell_common_defs.h>
#include <nsh/shell_config.h>
#include <stdbool.h>

typedef struct shell_line_buffer
{
	char buffer[SHELL_LINE_BUFFER_SIZE];
	int size;
} shell_line_buffer_t;

void shell_line_buffer_reset(shell_line_buffer_t *linebuf);

int shell_line_buffer_append_char(shell_line_buffer_t *linebuf, char c);

void shell_line_buffer_append_null(shell_line_buffer_t *linebuf);

void shell_line_buffer_erase_last_char(shell_line_buffer_t *linebuf);

bool shell_line_buffer_is_full(shell_line_buffer_t *linebuf);

bool shell_line_buffer_is_empty(shell_line_buffer_t *linebuf);

#endif /* SHELL_LINE_BUFFER_H_ */
