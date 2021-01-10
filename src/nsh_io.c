#include <nsh/nsh_config.h>
#include <nsh/nsh_io.h>

#include <stdio.h>
#include <string.h>
#if NSH_FEATURE_USE_PRINTF == 1
#include <stdarg.h>
#endif /* NSH_FEATURE_USE_PRINTF == 1 */

#define NSH_IO_ESC             "\e"
#define NSH_IO_CSI             NSH_IO_ESC "["
#define NSH_IO_ERASE_LINE      NSH_IO_CSI "2K"
#define NSH_IO_MOVE_BEGIN_LINE "\r"

char nsh_io_get_char(void)
{
    return getchar();
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
    // TODO This implementation is suboptimal and two loops are executed
    // One by strlen, one by nsh_io_put_buffer...
    nsh_io_put_buffer(str, (int)strlen(str));
}

void nsh_io_put_buffer(const char* str, int size)
{
    for (int i = 0; i < size; i++) {
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
int nsh_io_printf(const char* restrict format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);
    return ret;
}
#endif /* NSH_FEATURE_USE_PRINTF == 1 */
