#ifndef NSH_IO_PLUGIN_H_
#define NSH_IO_PLUGIN_H_

#include <nsh/nsh_config.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef char nsh_io_get_char_t(void);
typedef void nsh_io_put_char_t(char c);
typedef void nsh_io_put_newline_t(void);
typedef void nsh_io_put_string_t(const char* str);
typedef void nsh_io_put_buffer_t(const char* str, unsigned int size);
typedef void nsh_io_print_prompt_t(void);
typedef void nsh_io_erase_last_char_t(void);
typedef void nsh_io_erase_line_t(void);
#if NSH_FEATURE_USE_PRINTF == 1
typedef int nsh_io_printf_t(const char* NSH_RESTRICT format, ...);
#endif

typedef struct nsh_io_plugin {
    nsh_io_get_char_t* get_char;
    nsh_io_put_char_t* put_char;
    nsh_io_put_newline_t* put_newline;
    nsh_io_put_string_t* put_string;
    nsh_io_put_buffer_t* put_buffer;
    nsh_io_print_prompt_t* print_prompt;
    nsh_io_erase_last_char_t* erase_last_char;
    nsh_io_erase_line_t* erase_line;
#if NSH_FEATURE_USE_PRINTF == 1
    nsh_io_printf_t* printf;
#endif
} nsh_io_plugin_t;

#ifdef __cplusplus
}
#endif

#endif // NSH_IO_PLUGIN_H_
