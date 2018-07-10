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
#if SHELL_FEATURE_USE_HISTORY == 1
	shell_history_t history;
	int current_history_entry;
#endif

} shell_t;

static shell_t shell;

int shell_init(void)
{
	shell.cmds.count = 0;

    shell_line_buffer_reset(&shell.line);

#if SHELL_FEATURE_USE_HISTORY == 1
    shell_history_reset(&shell.history);
	shell.current_history_entry = -1;
#endif

	shell_cmd_register(&shell.cmds, "help", cmd_builtin_help);
	shell_cmd_register(&shell.cmds, "held", cmd_builtin_help);
	shell_cmd_register(&shell.cmds, "hold", cmd_builtin_help);

	return SHELL_STATUS_OK;
}

static int shell_execute(int argc, char **argv)
{
    if (argv[0] == NULL || argv[0][0] == '\0')
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
    {
        // Else execute matching command
    	int ret = matching_cmd->handler(argc, argv);
#if SHELL_FEATURE_USE_RETURN_CODE_PRINTING == 1
    	shell_io_printf("command '%s' return %d\r\n", argv[0], ret);
#endif
    	return SHELL_STATUS_OK;
    }
}

static int shell_autocomplete(void)
{
#if SHELL_FEATURE_USE_AUTOCOMPLETION == 0
    return SHELL_STATUS_UNSUPPORTED;
#else
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

    return SHELL_STATUS_OK;
#endif
}

#if SHELL_FEATURE_USE_HISTORY == 1
static void shell_display_history_entry(int age)
{
    if (age == -1)
    {
        shell_io_erase_line();
        shell_io_print_prompt();
    }
    else
    {
        int status = shell_history_get_entry(&shell.history, age, shell.line.buffer);
        if (status == SHELL_STATUS_OK)
        {
            shell_io_erase_line();
            shell_io_print_prompt();
            shell_io_put_string(shell.line.buffer);
            shell.line.size = strlen(shell.line.buffer);
        }
    }
}
#endif

static int shell_display_previous_entry(void)
{
#if SHELL_FEATURE_USE_HISTORY == 0
    return SHELL_STATUS_UNSUPPORTED;
#else
    shell.current_history_entry++;
    int entry_count = shell_history_entry_count(&shell.history);
    if(shell.current_history_entry >= entry_count)
        shell.current_history_entry = entry_count-1;

    shell_display_history_entry(shell.current_history_entry);

    return SHELL_STATUS_OK;
#endif
}

static int shell_display_next_entry(void)
{
#if SHELL_FEATURE_USE_HISTORY == 0
    return SHELL_STATUS_UNSUPPORTED;
#else
    if (shell.current_history_entry >= 0)
        shell.current_history_entry--;
    
   shell_display_history_entry(shell.current_history_entry);

   return SHELL_STATUS_OK;
#endif
}

static int shell_handle_escape_sequence(void)
{
    // Only VT100 escape sequences with the form "\e[<code>" are supported

    // We assume '\e' has been handled already, so we just ignore '['
    shell_io_get_char();

    // Handle escaped code
    char c = shell_io_get_char();
    switch (c)
    {
    case 'A': // Arrow up
        return shell_display_previous_entry();

    case 'B': // Arrow down
        return shell_display_next_entry();

    case 'C': // Arrow right
    case 'D': // Arrow left
    default:
        return SHELL_STATUS_UNSUPPORTED;
    }
}

static void shell_validate_entry(void)
{
    // ensure the line buffer is null-terminated
    shell_line_buffer_append_null(&shell.line);

#if SHELL_FEATURE_USE_HISTORY == 1
    // if the entry is not empty, add it into history
    if (!shell_line_buffer_is_empty(&shell.line))
        shell_history_add_entry(&shell.history, shell.line.buffer);
#endif

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
#if SHELL_FEATURE_USE_HISTORY == 1
    shell.current_history_entry = -1;
#endif

    shell_line_buffer_reset(&shell.line);

    while (true)
    {
        char c = shell_io_get_char();
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

        if (shell_line_buffer_is_full(&shell.line))
        {
            shell_io_put_newline();
            shell_io_put_string("WARNING: line buffer reach its maximum capacity\r\n");
            break;
        }
    }

    return SHELL_STATUS_BUFFER_OVERFLOW;
}

static int shell_copy_token(const char *str, char output[][SHELL_MAX_STRING_SIZE], int *token_count, int token_size)
{
    if (token_size > SHELL_MAX_STRING_SIZE-1) // Keep one char for '\0'
    {
        shell_io_put_string("WARNING: too long argument\r\n");
        return SHELL_STATUS_BUFFER_OVERFLOW;
    }
    memcpy(output[*token_count], str, token_size);
    output[*token_count][token_size] = '\0';
    (*token_count)++;
    if (*token_count > SHELL_CMD_ARGS_MAX_COUNT)
    {
        shell_io_put_string("WARNING: too many arguments\r\n");
        return SHELL_STATUS_MAX_ARGS_NB_REACH;
    }
    return SHELL_STATUS_OK;
}

static int shell_split_command_line(const char *str, char sep, char output[][SHELL_MAX_STRING_SIZE], int *token_count)
{
    int beg = 0;
    int end = 0;
    int input_size = strlen(str);

    *token_count = 0;

    for (int i = 0; i < input_size; i++)
    {
        if (str[i] == sep)
        {
            end = i;
            int ret = shell_copy_token(&str[beg], output, token_count, end-beg);
            if (ret != SHELL_STATUS_OK)
                // If an error is detected during copy, abort split and return the error code
                return ret;
            end++;
            beg = end;
        }
    }

    // Parse the last token and return the status of the copy
    return shell_copy_token(&str[beg], output, token_count, input_size-beg);
}


int shell_register_command(const char *name, shell_cmd_handler_t handler)
{
	shell_status_t status = shell_cmd_register(&shell.cmds, name, handler);
	return status;
}


void run_shell(void)
{
    // Local storage for command line after split
    char args[SHELL_CMD_ARGS_MAX_COUNT][SHELL_MAX_STRING_SIZE];

    // Array of pointers that point to the string contained by 'args'
    // In C/C++, an array decays to a pointer in most circumstances,
    // but this isn't recursive. So a T[] decays to a T*, but a T[][]
    // doesn't decay to a T**. Then we are forced to an extra step:
    // char[][] -> char*[] -> char**
    char *argv[SHELL_CMD_ARGS_MAX_COUNT];


    while (true)
    {
        int argc = 0;

        shell_io_print_prompt();

        // Read a command line and store it into 'shell.line.buffer'
        shell_status_t status = shell_read_line();

        if (status == SHELL_STATUS_OK)
        {
            // Split the command line into argument tokens
            if (shell_split_command_line(shell.line.buffer, ' ', args, &argc) != SHELL_STATUS_OK)
                // Ignore this command since there was an error
                continue;

            // Copy the address of all tokens into 'argv'
            for (int i = 0; i < argc; i++)
                argv[i] = args[i];

            // Execute the command with 'argc' number of argument stored in 'argv'
            shell_execute(argc, argv);
        }
    }
}
