#include <nsh/nsh_line_buffer.h>

#include <stdio.h>

void nsh_line_buffer_reset(nsh_line_buffer_t* linebuf)
{
    linebuf->size = 0;
    linebuf->buffer[0] = '\0';
}

nsh_status_t nsh_line_buffer_append_char(nsh_line_buffer_t* linebuf, char c)
{
    if (nsh_line_buffer_is_full(linebuf)) {
        return NSH_STATUS_BUFFER_OVERFLOW;
    }

    linebuf->buffer[linebuf->size++] = c;
    return NSH_STATUS_OK;
}

void nsh_line_buffer_append_null(nsh_line_buffer_t* linebuf)
{
    if (nsh_line_buffer_is_full(linebuf)) {
        linebuf->buffer[NSH_LINE_BUFFER_SIZE - 1] = '\0'; // overwrite last char to ensure the buffer is null terminated
    } else {
        nsh_line_buffer_append_char(linebuf, '\0');
    }
}

void nsh_line_buffer_erase_last_char(nsh_line_buffer_t* linebuf)
{
    if (!nsh_line_buffer_is_empty(linebuf)) {
        linebuf->size--;
    }
}

bool nsh_line_buffer_is_full(nsh_line_buffer_t* linebuf)
{
    return (linebuf->size >= NSH_LINE_BUFFER_SIZE);
}

bool nsh_line_buffer_is_empty(nsh_line_buffer_t* linebuf)
{
    return (linebuf->size == 0) || (linebuf->buffer[0] == '\0');
}
