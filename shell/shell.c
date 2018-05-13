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
    shell_line_buffer_t line;
	shell_cmd_array_t cmds;
	shell_history_t history;
	int current_history_entry;

} shell_t;

static shell_t shell;

int shell_init(void)
{
	shell.cmds.count = 0;
	shell.current_history_entry = -1;
    shell_line_buffer_reset(&shell.line);
	shell_history_reset(&shell.history);
	shell_cmd_register(&shell.cmds, "help", cmd_builtin_help);
	shell_cmd_register(&shell.cmds, "held", cmd_builtin_help);
	shell_cmd_register(&shell.cmds, "hold", cmd_builtin_help);
	return SHELL_STATUS_OK;
}

static int shell_execute(int argc, char **argv)
{
    if (argv[0] == NULL)
        // An empty command was entered.
        return SHELL_STATUS_EMPTY_CMD;

    // Find matching command
    shell_cmd_t *matching_cmd = shell_cmd_find(&shell.cmds, argv[0]);

    if (!matching_cmd)
        // If there is no match, return an error
        return SHELL_STATUS_CMD_NOT_FOUND;
    else if (!matching_cmd->handler)
    	// If handler is null, return an error
    	return SHELL_STATUS_EMPTY_CMD;
    else
        // Else execute matching command
    	return matching_cmd->handler(argc, argv);
}

static void shell_autocomplete(void)
{
    shell_cmd_t match[shell.cmds.count];
    int match_count = 0;

    // Find all commands name matching the actual buffer
    for (int i = 0; i < shell.cmds.count; i++)
        if (memcmp(shell.line.buffer, shell.cmds.array[i].name, shell.line.size) == 0)
            shell_cmd_copy(&match[match_count++], &shell.cmds.array[i]);

    if (match_count > 0)
    {
        // Sort the matching commands in lexicographical order
        shell_cmd_lexicographic_sort(&shell.cmds);

        // Display the commands name
        shell_io_put_newline();
        for (int i = 0; i < match_count; i++)
        {
            shell_io_put_string(match[i].name);
            shell_io_put_char(' ');
        }
    }

    // Print the prompt again
    shell_io_put_newline();
    shell_io_print_prompt();

    // Reprint the current buffer
    shell_io_put_buffer(shell.line.buffer, shell.line.size);
}

static void shell_display_previous_entry(void)
{
    shell.current_history_entry++;
    if(shell.current_history_entry > SHELL_CMD_HISTORY_SIZE)
        shell.current_history_entry = SHELL_CMD_HISTORY_SIZE-1;

	int status = shell_history_get_entry(&shell.history, shell.current_history_entry, shell.line.buffer);
	if (status == SHELL_STATUS_OK)
	{
	    shell_io_erase_line();
	    shell_io_print_prompt();
		shell_io_put_string(shell.line.buffer);
		shell.line.size = strlen(shell.line.buffer);
	}
}

static void shell_display_next_entry(void)
{

}

static void shell_handle_escape_sequence(void)
{
    // Ignore '['
    shell_io_get_char();
    // Handle escaped code
    char c = shell_io_get_char();
    switch (c)
    {
    case 'A': // Arrow up
        shell_display_previous_entry();
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
    shell_io_put_newline();
}

static void shell_erase_last_char(void)
{
	if (!shell_line_buffer_is_empty(&shell.line))
	{
		shell_io_erase_last_char();
		shell_line_buffer_erase_last_char(&shell.line);
	}
}

static int shell_read_line(void)
{
    char c;
    //int hist_index = -1;
    shell.current_history_entry = -1;

    shell_line_buffer_reset(&shell.line);

    while (!shell_line_buffer_is_full(&shell.line))
    {
        c = shell_io_get_char();
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
        	shell_erase_last_char();
            continue;
        case '\x1b':
            shell_handle_escape_sequence();
            continue;
        default:
            shell_io_put_char(c);
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
        shell_io_print_prompt();
        status = shell_read_line();
        //printf("buf='%s'\r\n", buf);
    }
}
