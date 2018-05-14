#include "shell_config.h"
#include "shell_io.h"
#include <stdio.h>
#include <string.h>
#if SHELL_FEATURE_USE_PRINTF == 1
#include <stdarg.h>
#endif

#define SHELL_IO_ESC 				"\e"
#define SHELL_IO_CSI 				SHELL_IO_ESC "["
#define SHELL_IO_ERASE_LINE 		SHELL_IO_CSI "2K"
#define SHELL_IO_MOVE_BEGIN_LINE 	"\r"

char shell_io_get_char(void)
{
    return getchar();
}

void shell_io_put_char(char c)
{
    putchar(c);
}

void shell_io_put_newline(void)
{
    shell_io_put_char('\r');
    shell_io_put_char('\n');
}

void shell_io_put_string(const char *str)
{
    shell_io_put_buffer(str, strlen(str));
}

void shell_io_put_buffer(const char *str, int size)
{
    for (int i = 0; i < size; i++)
        shell_io_put_char(str[i]);
}

void shell_io_print_prompt(void)
{
    shell_io_put_string(SHELL_DEFAULT_PROMPT);
}

void shell_io_erase_last_char(void)
{
    // go back to one character
    putchar('\b');
    // overwrite the char with whitespace
    putchar(' ');
    // go back to the now removed char position
    putchar('\b');
}

void shell_io_erase_line(void)
{
	printf(SHELL_IO_ERASE_LINE);
	printf(SHELL_IO_MOVE_BEGIN_LINE);
}

#if SHELL_FEATURE_USE_PRINTF == 1
int shell_io_printf(const char *restrict format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);
    return ret;
}
#endif
