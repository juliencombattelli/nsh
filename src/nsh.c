#include <nsh/nsh.h>
#include <nsh/nsh_cmd.h>
#include <nsh/nsh_cmd_builtins.h>
#include <nsh/nsh_common_defs.h>
#include <nsh/nsh_history.h>
#include <nsh/nsh_io.h>
#include <nsh/nsh_line_buffer.h>

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
//// Shell structure/instance declaration
///////////////////////////////////////////////////////////////////////////////

typedef struct nsh {
    nsh_line_buffer_t line;
    nsh_cmd_array_t cmds;
#if NSH_FEATURE_USE_HISTORY == 1
    nsh_history_t history;
    int current_history_entry;
#endif

} nsh_t;

static nsh_t nsh;

int nsh_init(void)
{
    nsh.cmds.count = 0;

    nsh_line_buffer_reset(&nsh.line);

#if NSH_FEATURE_USE_HISTORY == 1
    nsh_history_reset(&nsh.history);
    nsh.current_history_entry = -1;
#endif

    nsh_cmd_register(&nsh.cmds, "help", cmd_builtin_help);
    nsh_cmd_register(&nsh.cmds, "exit", cmd_builtin_exit);

    return NSH_STATUS_OK;
}

static int nsh_execute(int argc, char** argv)
{
    if (argv[0] == NULL || argv[0][0] == '\0') {
        // An empty command was entered.
        return NSH_STATUS_EMPTY_CMD;
    }

    // Find matching command
    nsh_cmd_t* matching_cmd = nsh_cmd_find(&nsh.cmds, argv[0]);

    if (!matching_cmd) {
        // If there is no match, return an error
        return NSH_STATUS_CMD_NOT_FOUND;
    }
    if (!matching_cmd->handler) {
        // If handler is null, return an error
        return NSH_STATUS_EMPTY_CMD;
    }

    // Execute matching command
    int ret = matching_cmd->handler(argc, argv);
#if NSH_FEATURE_USE_RETURN_CODE_PRINTING == 1
    nsh_io_printf("command '%s' return %d\r\n", argv[0], ret);
#endif
    return ret;
}

static int nsh_autocomplete(void)
{
#if NSH_FEATURE_USE_AUTOCOMPLETION == 0
    return NSH_STATUS_UNSUPPORTED;
#else

    nsh_cmd_array_t match;
    match.count = 0;

    // Find all commands name matching the actual buffer
    for (int i = 0; i < nsh.cmds.count; i++) {
        if (memcmp(nsh.line.buffer, nsh.cmds.array[i].name, nsh.line.size) == 0) {
            nsh_cmd_copy(&match.array[match.count++], &nsh.cmds.array[i]);
        }
    }

    if (match.count > 0) {
        // Sort the matching commands in lexicographical order
        nsh_cmd_lexicographic_sort(&match);

        // Display the commands name
        nsh_io_put_newline();
        for (int i = 0; i < match.count; i++) {
            nsh_io_put_string(match.array[i].name);
            nsh_io_put_char(' ');
        }
    }

    // Print the prompt again
    nsh_io_put_newline();
    nsh_io_print_prompt();

    // Reprint the current buffer
    nsh_io_put_buffer(nsh.line.buffer, nsh.line.size);

    return NSH_STATUS_OK;
#endif
}

#if NSH_FEATURE_USE_HISTORY == 1
static void nsh_display_history_entry(int age)
{
    if (age == -1) {
        nsh_io_erase_line();
        nsh_io_print_prompt();
    } else {
        int status = nsh_history_get_entry(&nsh.history, age, nsh.line.buffer);
        if (status == NSH_STATUS_OK) {
            nsh_io_erase_line();
            nsh_io_print_prompt();
            nsh_io_put_string(nsh.line.buffer);
            nsh.line.size = (int)strlen(nsh.line.buffer);
        }
    }
}
#endif

static int nsh_display_previous_entry(void)
{
#if NSH_FEATURE_USE_HISTORY == 0
    return NSH_STATUS_UNSUPPORTED;
#else
    nsh.current_history_entry++;
    int entry_count = nsh_history_entry_count(&nsh.history);
    if (nsh.current_history_entry >= entry_count) {
        nsh.current_history_entry = entry_count - 1;
    }

    nsh_display_history_entry(nsh.current_history_entry);

    return NSH_STATUS_OK;
#endif
}

static int nsh_display_next_entry(void)
{
#if NSH_FEATURE_USE_HISTORY == 0
    return NSH_STATUS_UNSUPPORTED;
#else
    if (nsh.current_history_entry >= 0) {
        nsh.current_history_entry--;
    }

    nsh_display_history_entry(nsh.current_history_entry);

    return NSH_STATUS_OK;
#endif
}

static int nsh_handle_escape_sequence(void)
{
    // Only VT100 escape sequences with the form "\e[<code>" are supported

    // We assume '\e' has been handled already, so we just ignore '['
    nsh_io_get_char();

    // Handle escaped code
    char c = nsh_io_get_char();
    switch (c) {
    case 'A': // Arrow up
        return nsh_display_previous_entry();

    case 'B': // Arrow down
        return nsh_display_next_entry();

    case 'C': // Arrow right
    case 'D': // Arrow left
    default:
        return NSH_STATUS_UNSUPPORTED;
    }
}

static void nsh_validate_entry(void)
{
    // ensure the line buffer is null-terminated
    nsh_line_buffer_append_null(&nsh.line);

#if NSH_FEATURE_USE_HISTORY == 1
    // if the entry is not empty, add it into history
    if (!nsh_line_buffer_is_empty(&nsh.line)) {
        nsh_history_add_entry(&nsh.history, nsh.line.buffer);
    }
#endif

    // print newline
    nsh_io_put_newline();
}

static void nsh_erase_last_char(void)
{
    if (!nsh_line_buffer_is_empty(&nsh.line)) {
        nsh_io_erase_last_char();
        nsh_line_buffer_erase_last_char(&nsh.line);
    }
}

static int nsh_read_line(void)
{
#if NSH_FEATURE_USE_HISTORY == 1
    nsh.current_history_entry = -1;
#endif

    nsh_line_buffer_reset(&nsh.line);

    while (true) {
        char c = nsh_io_get_char();
        switch (c) {
        case '\r':
        case '\n':
            nsh_validate_entry();
            return NSH_STATUS_OK;
        case '\t':
            nsh_autocomplete();
            continue;
        case '\b':
            nsh_erase_last_char();
            continue;
        case '\x1b':
            nsh_handle_escape_sequence();
            continue;
        default:
            nsh_io_put_char(c);
            nsh_line_buffer_append_char(&nsh.line, c);
        }

        if (nsh_line_buffer_is_full(&nsh.line)) {
            nsh_io_put_newline();
            nsh_io_put_string("WARNING: line buffer reach its maximum capacity\r\n");
            break;
        }
    }

    return NSH_STATUS_BUFFER_OVERFLOW;
}

static int nsh_copy_token(const char* str, char output[][NSH_MAX_STRING_SIZE], int* token_count, int token_size)
{
    if (token_size > NSH_MAX_STRING_SIZE - 1) // Keep one char for '\0'
    {
        nsh_io_put_string("WARNING: too long argument\r\n");
        return NSH_STATUS_BUFFER_OVERFLOW;
    }
    memcpy(output[*token_count], str, token_size);
    output[*token_count][token_size] = '\0';
    (*token_count)++;
    if (*token_count > NSH_CMD_ARGS_MAX_COUNT) {
        nsh_io_put_string("WARNING: too many arguments\r\n");
        return NSH_STATUS_MAX_ARGS_NB_REACH;
    }
    return NSH_STATUS_OK;
}

static int nsh_split_command_line(const char* str, char sep, char output[][NSH_MAX_STRING_SIZE], int* token_count)
{
    int beg = 0;
    int end = 0;
    int input_size = strlen(str);

    *token_count = 0;

    for (int i = 0; i < input_size; i++) {
        if (str[i] == sep) {
            end = i;
            int ret = nsh_copy_token(&str[beg], output, token_count, end - beg);
            if (ret != NSH_STATUS_OK) { // If an error is detected during copy, abort split and return the error code
                return ret;
            }
            end++;
            beg = end;
        }
    }

    // Parse the last token and return the status of the copy
    return nsh_copy_token(&str[beg], output, token_count, input_size - beg);
}

int nsh_register_command(const char* name, nsh_cmd_handler_t handler)
{
    nsh_status_t status = nsh_cmd_register(&nsh.cmds, name, handler);
    return status;
}

void nsh_run(void)
{
    // Local storage for command line after spliting
    char args[NSH_CMD_ARGS_MAX_COUNT][NSH_MAX_STRING_SIZE];

    // Array of pointers that point to the string contained by 'args'
    // In C/C++, an array decays to a pointer in most circumstances,
    // but this isn't recursive. So a T[] decays to a T*, but a T[][]
    // doesn't decay to a T**. Then we are forced to add an extra step:
    // char[][] -> char*[] -> char**
    char* argv[NSH_CMD_ARGS_MAX_COUNT] = { NULL };

    while (true) {
        int argc = 0;

        nsh_io_print_prompt();

        // Read a command line and store it into 'nsh.line.buffer'
        nsh_status_t status = nsh_read_line();

        if (status == NSH_STATUS_OK) {
            // Split the command line into argument tokens
            if (nsh_split_command_line(nsh.line.buffer, ' ', args, &argc) != NSH_STATUS_OK) {
                // Ignore this command since there was an error
                continue;
            }

            // Copy the address of all tokens into 'argv'
            for (int i = 0; i < argc; i++) {
                argv[i] = args[i];
            }

            // Execute the command with 'argc' number of argument stored in 'argv'
            int cmd_result = nsh_execute(argc, argv);
            if (cmd_result == NSH_STATUS_CMD_NOT_FOUND) {
                nsh_io_put_string("ERROR: command '");
                nsh_io_put_string(argv[0]);
                nsh_io_put_string("' not found\r\n");
            } else if (cmd_result == NSH_STATUS_QUIT) {
                break;
            }
        }
    }
}
