#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "shell.h"
#include "shell_cmd.h"
#include "shell_history.h"
#include "shell_common_defs.h"
#include "shell_cmd_builtins.h"

typedef struct shell
{
	shell_cmd_t cmds[SHELL_CMD_MAX_COUNT];
	int cmd_count;
	shell_history_t history;

} shell_t;

static shell_t shell;

int shell_init()
{
	shell_history_reset(&shell.history);
	shell_register_command("help", cmd_builtin_help);
	shell_register_command("held", cmd_builtin_help);
	shell_register_command("hold", cmd_builtin_help);
	return SHELL_STATUS_OK;
}

int shell_register_command(const char *name, shell_cmd_handler_t handler)
{
    size_t name_len = strlen(name);
    if (name_len == 0 || name_len > SHELL_CMD_NAME_SIZE || !handler)
        return SHELL_STATUS_WRONG_ARG;

    strcpy(shell.cmds[shell.cmd_count].name, name);
    shell.cmds[shell.cmd_count].handler = handler;

    shell.cmd_count++;

    return SHELL_STATUS_OK;
}

static shell_cmd_t* shell_find_matching_cmd(const char *partial_name, int size)
{
    for (int i = 0; i < shell.cmd_count; i++)
        if (memcmp(partial_name, shell.cmds[i].name, size) == 0)
            return &shell.cmds[i];
    return NULL;
}

static shell_cmd_t* shell_find_cmd(const char *name)
{
    return shell_find_matching_cmd(name, strlen(name));
}

static void shell_print_prompt(void)
{
    printf(SHELL_DEFAULT_PROMPT);
}

static int shell_execute(int argc, char **argv)
{
    if (argv[0] == NULL)
    {
        // An empty command was entered.
        return SHELL_STATUS_EMPTY_CMD;
    }

    // Find matching command
    shell_cmd_t *matching_cmd = shell_find_cmd(argv[0]);

    if (!matching_cmd)
        // If there is no match, return an error
        return SHELL_STATUS_CMD_NOT_FOUND;
    else
        // Else execute matching command
        return (*(matching_cmd->handler))(argc, argv);
}

static void shell_lexicographic_cmd_sort(shell_cmd_t **cmds, int cmd_count)
{
    for (int i = 0; i < cmd_count - 1; ++i)
        for (int j = i + 1; j < cmd_count; ++j)
            if (strcmp(cmds[i]->name, cmds[j]->name) > 0)
                shell_cmdswap(cmds[i], cmds[j]);
}

static int shell_autocomplete(const char *buffer, int size)
{
    if (!buffer)
        return SHELL_STATUS_WRONG_ARG;

    shell_cmd_t *match[shell.cmd_count];
    int match_count = 0;

    // Find all commands name matching the actual buffer
    for (int i = 0; i < shell.cmd_count; i++)
        if (memcmp(buffer, shell.cmds[i].name, size) == 0)
            match[match_count++] = &shell.cmds[i];

    if (match_count > 0)
    {
        // Sort the matching commands in lexicographical order
        shell_lexicographic_cmd_sort(match, match_count);

        // Display the commands name
        printf("\r\n");
        for (int i = 0; i < match_count; i++)
            printf("%s ", match[i]->name);
    }

    // Print the prompt again
    printf("\r\n");
    shell_print_prompt();

    // Reprint the current buffer
    //   (at this step, the string [buffer, buffer+position] IS NOT null terminated)
    char current_buffer_str[size + 1];
    memcpy(current_buffer_str, buffer, size);
    current_buffer_str[size] = '\0';
    printf("%s", current_buffer_str);

    return SHELL_STATUS_OK;
}

static void shell_print_backspace(void)
{
    // go back to one character
    putchar('\b');
    // overwrite the char with whitespace
    putchar(' ');
    // go back to the now removed char position
    putchar('\b');
}

static int shell_read_line(char *buffer)
{
    int position = 0;
    char c;

    int hist_index = 0;

    if (!buffer)
        return SHELL_STATUS_WRONG_ARG;

    while (position < SHELL_LINE_BUFFER_SIZE)
    {
        c = getchar();

        // Handle return key
        if (c == '\r' || c == '\n')
        {
            buffer[position] = '\0';
            shell_history_add_entry(&shell.history, buffer);
            printf("\r\n");
            return SHELL_STATUS_OK;
        }
        // Handle tabulation
        else if (c == '\t')
        {
            shell_autocomplete(buffer, position);
        }
        // Handle backspace
        else if (c == '\b')
        {
            if (position > 0)
            {
                position--;
                shell_print_backspace();
            }
        }
        // Handle escape sequence
        else if (c == '\x1b')
        {
            // Ignore '['
            getchar();
            // Handle escaped code
            switch (getchar())
            {
            case 'A': // Arrow up
                if (!shell_history_is_empty(&shell.history))
                {
                    shell_history_get_entry(&shell.history, hist_index, buffer);

                    hist_index++;
                    if(hist_index > SHELL_CMD_HISTORY_SIZE)
                        hist_index = SHELL_CMD_HISTORY_SIZE-1;

                    for(int i = 0; i < position; i++)
                        shell_print_backspace();
                    printf("%s", buffer);
                    position = strlen(buffer);
                }
                break;

            case 'B': // Arrow down
                if (!shell_history_is_empty(&shell.history))
                {
                    hist_index--;
                    if(hist_index < 0)
                        hist_index = 0;

                    shell_history_get_entry(&shell.history, hist_index, buffer);

                    for(int i = 0; i < position; i++)
                        shell_print_backspace();
                    printf("%s", buffer);
                    position = strlen(buffer);
                }
                break;

            case 'C': // Arrow right
            case 'D': // Arrow left
                putchar(c);
                break;
            default:
                break;
            }
        }
        // Echo all others and append them to buffer
        else
        {
            putchar(c);
            buffer[position++] = c;
        }
    }

    return SHELL_STATUS_BUFFER_OVERFLOW;
}

/*void shell_split(const char *input, char **output)
 void split(const char *str, char sep, split_fn fun, void *data)
 {
 unsigned int start = 0, stop;
 for (stop = 0; str[stop]; stop++)
 {
 if (str[stop] == sep)
 {
 fun(str + start, stop - start, data);
 start = stop + 1;
 }
 }
 fun(str + start, stop - start, data);
 }*/

void run_shell(void)
{
    shell_status_t status = SHELL_STATUS_OK;

    char buf[SHELL_LINE_BUFFER_SIZE];

    shell_init();

    while (status == SHELL_STATUS_OK)
    {
        shell_print_prompt();
        status = shell_read_line(buf);
        //printf("buf='%s'\r\n", buf);
    }
}
