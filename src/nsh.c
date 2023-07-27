#include <nsh/nsh.h>
#include <nsh/nsh_cmd_builtins.h>
#include <nsh/nsh_common_defs.h>
#include <nsh/nsh_io_plugin.h>

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static nsh_status_t nsh_copy_token(const char* str, char output[][NSH_MAX_STRING_SIZE], unsigned int* token_count,
    unsigned int token_size)
    NSH_NON_NULL(1, 2, 3);

static nsh_status_t nsh_split_command_line(const char* str, char sep, char output[][NSH_MAX_STRING_SIZE],
    unsigned int* token_count)
    NSH_NON_NULL(1, 3, 4);

static nsh_status_t nsh_execute(const nsh_t* nsh, unsigned int argc, char** argv)
    NSH_NON_NULL(1);

#if NSH_FEATURE_USE_AUTOCOMPLETION == 1

static nsh_status_t nsh_autocomplete(const nsh_t* nsh)
    NSH_NON_NULL(1);

#endif

#if NSH_FEATURE_USE_HISTORY == 1

static void nsh_display_history_entry(nsh_t* nsh)
    NSH_NON_NULL(1);

static nsh_status_t nsh_display_previous_entry(nsh_t* nsh)
    NSH_NON_NULL(1);

static nsh_status_t nsh_display_next_entry(nsh_t* nsh)
    NSH_NON_NULL(1);

#endif

static nsh_status_t nsh_handle_escape_sequence(nsh_t* nsh)
    NSH_NON_NULL(1);

static void nsh_validate_entry(nsh_t* nsh)
    NSH_NON_NULL(1);

static void nsh_erase_last_char(nsh_t* nsh)
    NSH_NON_NULL(1);

static nsh_status_t nsh_read_line(nsh_t* nsh)
    NSH_NON_NULL(1);

static nsh_status_t nsh_copy_token(const char* str, char output[][NSH_MAX_STRING_SIZE], unsigned int* token_count,
    unsigned int token_size)
{
    if (token_size > NSH_MAX_STRING_SIZE - 1) // Keep one char for '\0'
    {
        // nsh_io_put_string("WARNING: too long argument\r\n");
        return NSH_STATUS_BUFFER_OVERFLOW;
    }
    memcpy(output[*token_count], str, token_size);
    output[*token_count][token_size] = '\0';
    (*token_count)++;
    if (*token_count >= NSH_CMD_ARGS_MAX_COUNT) {
        // nsh_io_put_string("WARNING: too many arguments\r\n");
        return NSH_STATUS_MAX_ARGS_NB_REACH;
    }
    return NSH_STATUS_OK;
}

static nsh_status_t nsh_split_command_line(const char* str, char sep, char output[][NSH_MAX_STRING_SIZE],
    unsigned int* token_count)
{
    unsigned int beg = 0;
    unsigned int end = 0;
    unsigned int input_size = (unsigned int)strlen(str);

    *token_count = 0;

    for (unsigned int i = 0; i < input_size; i++) {
        if (str[i] == sep) {
            end = i;
            nsh_status_t ret = nsh_copy_token(&str[beg], output, token_count, end - beg);
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

static nsh_status_t nsh_execute(const nsh_t* nsh, unsigned int argc, char** argv)
{
    if (argv[0] == NULL || argv[0][0] == '\0') {
        // An empty command was entered.
        return NSH_STATUS_EMPTY_CMD;
    }

    // Find matching command
    const nsh_cmd_t* matching_cmd = nsh_cmd_array_find(&nsh->cmds, argv[0]);

    if (!matching_cmd) {
        // If there is no match, return an error
        return NSH_STATUS_CMD_NOT_FOUND;
    }
    if (!matching_cmd->handler) {
        // If handler is null, return an error
        return NSH_STATUS_EMPTY_CMD;
    }

    // Execute matching command
    nsh_status_t status = matching_cmd->handler(argc, argv);
#if NSH_FEATURE_USE_RETURN_CODE_PRINTING == 1
    nsh_io_printf("command '%s' return %d\r\n", argv[0], status);
#endif
    return status;
}

#if NSH_FEATURE_USE_AUTOCOMPLETION == 1

static nsh_status_t nsh_autocomplete(const nsh_t* nsh)
{
    nsh_cmd_array_t match;
    nsh_cmd_array_init(&match);

    // Find all commands name matching the actual buffer
    for (unsigned int i = 0; i < nsh->cmds.count; i++) {
        if (memcmp(nsh->line.buffer, nsh->cmds.array[i].name, nsh->line.size) == 0) {
            nsh_cmd_copy(&match.array[match.count++], &nsh->cmds.array[i]);
        }
    }

    if (match.count > 0) {
        // Sort the matching commands in lexicographical order
        nsh_cmd_array_lexicographic_sort(&match);

        // Display the commands name
        nsh_io_put_newline();
        for (unsigned int i = 0; i < match.count; i++) {
            nsh_io_put_string(match.array[i].name);
            nsh_io_put_char(' ');
        }
    }

    // Print the prompt again
    nsh_io_put_newline();
    nsh_io_print_prompt();

    // Reprint the current buffer
    nsh_io_put_buffer(nsh->line.buffer, nsh->line.size);

    return NSH_STATUS_OK;
}

#endif

#if NSH_FEATURE_USE_HISTORY == 1

static void nsh_display_history_entry(nsh_t* nsh)
{
    if (nsh->current_history_entry == NSH_HISTORY_INVALID_ENTRY) {
        nsh_io_erase_line();
        nsh_io_print_prompt();
        nsh_line_buffer_reset(&nsh->line);
    } else {
        nsh_status_t status = nsh_history_get_entry(&nsh->history, nsh->current_history_entry, nsh->line.buffer);
        if (status == NSH_STATUS_OK) {
            nsh_io_erase_line();
            nsh_io_print_prompt();
            nsh_io_put_string(nsh->line.buffer);
            nsh->line.size = (unsigned int)strlen(nsh->line.buffer);
        }
    }
}

static nsh_status_t nsh_display_previous_entry(nsh_t* nsh)
{
    nsh->current_history_entry++;
    unsigned int entry_count = nsh_history_entry_count(&nsh->history);
    if (nsh->current_history_entry >= entry_count) {
        nsh->current_history_entry = entry_count - 1;
    }

    nsh_display_history_entry(nsh);

    return NSH_STATUS_OK;
}

static nsh_status_t nsh_display_next_entry(nsh_t* nsh)
{
    if (nsh->current_history_entry < NSH_CMD_HISTORY_SIZE) {
        nsh->current_history_entry--;
    }

    nsh_display_history_entry(nsh);

    return NSH_STATUS_OK;
}

#endif

static nsh_status_t nsh_handle_escape_sequence(nsh_t* nsh)
{
#if NSH_FEATURE_USE_HISTORY == 0
    NSH_UNUSED(nsh);
#endif
    // Only VT100 escape sequences with the form "\e[<code>" are supported

    // We assume '\e' has been handled already, so we just ignore '['
    nsh_io_get_char();

    // Handle escaped code
    char c = nsh_io_get_char();
    switch (c) {
#if NSH_FEATURE_USE_HISTORY == 1
    case 'A': // Arrow up
        return nsh_display_previous_entry(nsh);
    case 'B': // Arrow down
        return nsh_display_next_entry(nsh);
#endif
    case 'C': // Arrow right
    case 'D': // Arrow left
    default:
        return NSH_STATUS_UNSUPPORTED;
    }
}

static void nsh_validate_entry(nsh_t* nsh)
{
    // ensure the line buffer is null-terminated
    nsh_line_buffer_append_null(&nsh->line);

#if NSH_FEATURE_USE_HISTORY == 1
    // if the entry is not empty, add it into history
    if (!nsh_line_buffer_is_empty(&nsh->line)) {
        nsh_history_add_entry(&nsh->history, nsh->line.buffer);
    }
#endif

    // print newline
    nsh_io_put_newline();
}

static void nsh_erase_last_char(nsh_t* nsh)
{
    if (!nsh_line_buffer_is_empty(&nsh->line)) {
        nsh_io_erase_last_char();
        nsh_line_buffer_erase_last_char(&nsh->line);
    }
}

static nsh_status_t nsh_read_line(nsh_t* nsh)
{
#if NSH_FEATURE_USE_HISTORY == 1
    nsh->current_history_entry = NSH_HISTORY_INVALID_ENTRY;
#endif

    nsh_line_buffer_reset(&nsh->line);

    while (true) {
        char c = nsh_io_get_char();
        switch (c) {
        case '\r':
        case '\n':
            nsh_validate_entry(nsh);
            return NSH_STATUS_OK;
#if NSH_FEATURE_USE_AUTOCOMPLETION == 1
        case '\t':
            nsh_autocomplete(nsh);
            continue;
#endif
        case '\b':
            nsh_erase_last_char(nsh);
            continue;
        case '\x1b':
            nsh_handle_escape_sequence(nsh);
            continue;
        default:
            nsh_io_put_char(c);
            nsh_line_buffer_append_char(&nsh->line, c);
        }

        if (nsh_line_buffer_is_full(&nsh->line)) {
            nsh_io_put_newline();
            nsh_io_put_string("WARNING: line buffer reach its maximum capacity\r\n");
            return NSH_STATUS_BUFFER_OVERFLOW;
        }
    }

    return NSH_STATUS_OK;
}

nsh_t nsh_init(nsh_status_t* status)
{
    nsh_t nsh = { 0 };

    nsh_cmd_array_init(&nsh.cmds);

    nsh_line_buffer_reset(&nsh.line);

#if NSH_FEATURE_USE_HISTORY == 1
    nsh_history_reset(&nsh.history);
    nsh.current_history_entry = NSH_HISTORY_INVALID_ENTRY;
#endif

    nsh_register_command(&nsh, "help", cmd_builtin_help);
    nsh_register_command(&nsh, "exit", cmd_builtin_exit);
    nsh_register_command(&nsh, "version", cmd_builtin_version);

    *status = NSH_STATUS_OK;

    return nsh;
}

nsh_status_t nsh_register_command(nsh_t* nsh, const char* name, nsh_cmd_handler_t* handler)
{
    return nsh_cmd_array_register(&nsh->cmds, name, handler);
}

void nsh_run(nsh_t* nsh)
{
    // Local storage for command line after spliting
    char args[NSH_CMD_ARGS_MAX_COUNT][NSH_MAX_STRING_SIZE];

    /*
     * Array of pointers that points to the string contained by 'args'.
     *
     * In C/C++, an array decays to a pointer in most circumstances,
     * but this isn't recursive. So a T[] decays to a T*, but a T[][]
     * doesn't decay to a T**. Therefore we are forced to add an extra
     * step: char[][] -> char*[] -> char**
     */
    char* argv[NSH_CMD_ARGS_MAX_COUNT] = { NULL };

    while (true) {
        unsigned int argc = 0;

        nsh_io_print_prompt();

        // Read a command line and store it into 'nsh->line.buffer'
        nsh_status_t status = nsh_read_line(nsh);

        if (status == NSH_STATUS_OK) {
            // Split the command line into argument tokens
            if (nsh_split_command_line(nsh->line.buffer, ' ', args, &argc) != NSH_STATUS_OK) {
                // Ignore this command since there was an error
                // TODO just print a warning to the user
                continue;
            }

            // Copy the address of all tokens into 'argv'
            for (unsigned int i = 0; i < argc; i++) {
                argv[i] = args[i];
            }

            // Execute the command with 'argc' number of argument stored in 'argv'
            nsh_status_t cmd_status = nsh_execute(nsh, argc, argv);
            if (cmd_status == NSH_STATUS_CMD_NOT_FOUND) {
                nsh_io_put_string("ERROR: command '");
                nsh_io_put_string(argv[0]);
                nsh_io_put_string("' not found\r\n");
            } else if (cmd_status == NSH_STATUS_QUIT) {
                break;
            }
        }
    }
}
