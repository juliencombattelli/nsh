#include "shell_config.h"
#include "shell_io.h"

char shell_get_char(void)
{
    return getchar();
}

void shell_put_char(char c)
{
    putchar(c);
}

void shell_put_newline(void)
{
    shell_put_char('\r');
    shell_put_char('\n');
}

void shell_put_string(const char *str)
{
    shell_put_buffer(str, strlen(str));
}

void shell_put_buffer(const char *str, int size)
{
    for (int i = 0; i < size; i++)
        shell_put_char(str[i]);
}

void shell_print_prompt(void)
{
    shell_put_string(SHELL_DEFAULT_PROMPT);
}

void shell_erase_last_char(void)
{
    // go back to one character
    putchar('\b');
    // overwrite the char with whitespace
    putchar(' ');
    // go back to the now removed char position
    putchar('\b');
}

void shell_erase_line(void)
{
    // TODO: use escape sequence
}
