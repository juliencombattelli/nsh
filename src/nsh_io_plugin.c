#include <nsh/nsh_common_defs.h>
#include <nsh/nsh_config.h>
#include <nsh/nsh_io_plugin.h>

#include <stdio.h>
#include <string.h>

#define NSH_IO_ESC             "\x1B"
#define NSH_IO_CSI             NSH_IO_ESC "["
#define NSH_IO_ERASE_LINE      NSH_IO_CSI "2K"
#define NSH_IO_MOVE_BEGIN_LINE "\r"

char nsh_io_get_char(void)
{
    int c;
    while ((c = getchar()) == EOF)
        ;
    return (char)c;
}

void nsh_io_put_char(char c)
{
    putchar(c);
}

void nsh_io_put_newline(void)
{
    nsh_io_put_char('\r');
    nsh_io_put_char('\n');
}

void nsh_io_put_string(const char* str)
{
    // TODO This implementation is suboptimal as two loops are executed:
    // one by strlen, one by nsh_io_put_buffer...
    // Migrating from null-terminated strings to mcsl's string_view could
    // solve this issue.
    nsh_io_put_buffer(str, (unsigned int)strlen(str));
}

void nsh_io_put_buffer(const char* str, unsigned int size)
{
    for (unsigned int i = 0; i < size; i++) {
        nsh_io_put_char(str[i]);
    }
}

void nsh_io_print_prompt(void)
{
    nsh_io_put_string(NSH_DEFAULT_PROMPT);
}

void nsh_io_erase_last_char(void)
{
    // go back to one character
    putchar('\b');
    // overwrite the char with whitespace
    putchar(' ');
    // go back to the now removed char position
    putchar('\b');
}

void nsh_io_erase_line(void)
{
    nsh_io_put_string(NSH_IO_ERASE_LINE);
    nsh_io_put_string(NSH_IO_MOVE_BEGIN_LINE);
}

#if NSH_FEATURE_USE_PRINTF == 1
#include <stdarg.h>
int nsh_io_printf(const char* NSH_RESTRICT format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);
    return ret;
}
#endif
