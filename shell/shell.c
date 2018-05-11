#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "shell.h"
#include "shell_io.h"
#include "shell_cmd.h"
#include "shell_line_buffer.h"
#include "shell_history.h"
#include "shell_common_defs.h"
#include "shell_cmd_builtins.h"

///////////////////////////////////////////////////////////////////////////////
//// Shell structure/instance declaration
///////////////////////////////////////////////////////////////////////////////

typedef struct shell
{
	shell_cmd_t cmds[SHELL_CMD_MAX_COUNT];
	int cmd_count;
	shell_history_t history;
	shell_line_buffer_t line;

} shell_t;

static shell_t shell;

int shell_init()
{
    shell_line_buffer_reset(&shell.line);
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

static void shell_autocomplete()
{
    shell_cmd_t *match[shell.cmd_count];
    int match_count = 0;

    // Find all commands name matching the actual buffer
    for (int i = 0; i < shell.cmd_count; i++)
        if (memcmp(shell.line.buffer, shell.cmds[i].name, shell.line.size) == 0)
            match[match_count++] = &shell.cmds[i];

    if (match_count > 0)
    {
        // Sort the matching commands in lexicographical order
        shell_lexicographic_cmd_sort(match, match_count);

        // Display the commands name
        shell_put_newline();
        for (int i = 0; i < match_count; i++)
        {
            shell_put_string(match[i]->name);
            shell_put_char(' ');
        }
    }

    // Print the prompt again
    shell_put_newline();
    shell_print_prompt();

    // Reprint the current buffer
    shell_put_buffer(shell.line.buffer, shell.line.size);

    /*//   (at this step, the string [buffer, buffer+position] IS NOT null terminated)
    char current_buffer_str[shell.line.size + 1];
    memcpy(current_buffer_str, shell.line.buffer, shell.line.size);
    current_buffer_str[shell.line.size] = '\0';
    printf("%s", current_buffer_str);*/
}

static void shell_handle_escape_sequence(void)
{
    // Ignore '['
    shell_get_char();
    // Handle escaped code
    char c = shell_get_char();
    switch (c)
    {
    case 'A': // Arrow up
        /*if (!shell_history_is_empty(&shell.history))
        {
            shell_history_get_entry(&shell.history, hist_index, shell.line.buffer);

            hist_index++;
            if(hist_index > SHELL_CMD_HISTORY_SIZE)
                hist_index = SHELL_CMD_HISTORY_SIZE-1;

            shell_erase_line_until_cursor(shell.line.size);
            printf("%s", buffer);
            position = strlen(buffer);
        }*/
        break;

    case 'B': // Arrow down
        /*if (!shell_history_is_empty(&shell.history))
        {
            hist_index--;
            if(hist_index < 0)
                hist_index = 0;

            shell_history_get_entry(&shell.history, hist_index, buffer);

            shell_erase_line_until_cursor(position);
            printf("%s", buffer);
            position = strlen(buffer);
        }*/
        break;

    case 'C': // Arrow right
    case 'D': // Arrow left
        //printf("\x1b[%c", c);
        break;
    default:
        break;
    }
}

static void shell_validate_entry(void)
{
    // ensure the line buffer is null-terminated
    shell_line_buffer_append_null(&shell.line);
    // add this validated entry into history
    shell_history_add_entry(&shell.history, shell.line.buffer);
    // print newline
    shell_put_newline();
}

static int shell_read_line(void)
{
    char c;
    //int hist_index = -1;

    shell_line_buffer_reset(&shell.line);

    while (!shell_line_buffer_is_full(&shell.line))
    {
        c = shell_get_char();
        switch(c)
        {
        case '\r':
        case '\n':
            shell_validate_entry();
            return SHELL_STATUS_OK;
        case '\t':
            shell_autocomplete();
            continue;
        case '\b':
            if (!shell_line_buffer_is_empty(&shell.line))
            {
                shell_erase_last_char();
                shell_line_buffer_erase_last_char(&shell.line);
            }
            continue;
        case '\x1b':
            shell_handle_escape_sequence();
            continue;
        default:
            shell_put_char(c);
            shell_line_buffer_append_char(&shell.line, c);
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

    shell_init();

    while (status == SHELL_STATUS_OK)
    {
        shell_print_prompt();
        status = shell_read_line();
        //printf("buf='%s'\r\n", buf);
    }
}
