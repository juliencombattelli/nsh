#include "shell_line_buffer.h"
#include <stdio.h>

void shell_line_buffer_reset(shell_line_buffer_t *linebuf)
{
	linebuf->size = 0;
	linebuf->buffer[0] = '\0';
}

int shell_line_buffer_append_char(shell_line_buffer_t *linebuf, char c)
{
	if (shell_line_buffer_is_full(linebuf))
		return SHELL_STATUS_BUFFER_OVERFLOW;

	linebuf->buffer[linebuf->size++] = c;
	return SHELL_STATUS_OK;
}

void shell_line_buffer_append_null(shell_line_buffer_t *linebuf)
{
	if (shell_line_buffer_is_full(linebuf))
		linebuf->buffer[SHELL_LINE_BUFFER_SIZE-1] = '\0'; // overwrite last char to ensure the buffer is null terminated
	else
		shell_line_buffer_append_char(linebuf, '\0');
}

void shell_line_buffer_erase_last_char(shell_line_buffer_t *linebuf)
{
	if (!shell_line_buffer_is_empty(linebuf))
		linebuf->size--;
}

bool shell_line_buffer_is_full(shell_line_buffer_t *linebuf)
{
	return (linebuf->size >= SHELL_LINE_BUFFER_SIZE);
}

bool shell_line_buffer_is_empty(shell_line_buffer_t *linebuf)
{
	return (linebuf->size == 0) || (linebuf->buffer[0] == '\0');
}
