#ifndef NSH_IO_PLUGIN_DEFAULT_H_
#define NSH_IO_PLUGIN_DEFAULT_H_

#include <nsh/nsh_io_plugin.h>

#ifdef __cplusplus
extern "C" {
#endif

char nsh_io_default_get_char(void);
void nsh_io_default_put_char(char c);
void nsh_io_default_put_newline(void);
void nsh_io_default_put_string(const char* str);
void nsh_io_default_put_buffer(const char* str, unsigned int size);
void nsh_io_default_print_prompt(void);
void nsh_io_default_erase_last_char(void);
void nsh_io_default_erase_line(void);
#if NSH_FEATURE_USE_PRINTF == 1
int nsh_io_default_printf(const char* NSH_RESTRICT format, ...);
#endif

#if NSH_IO_PLUGIN_IS_STATIC == 0
nsh_io_plugin_t nsh_io_make_default_plugin(void);
#else
static const nsh_io_plugin_t nsh_io_plugin_default = {
    .get_char = &nsh_io_default_get_char,
    .put_char = &nsh_io_default_put_char,
    .put_newline = &nsh_io_default_put_newline,
    .put_string = &nsh_io_default_put_string,
    .put_buffer = &nsh_io_default_put_buffer,
    .print_prompt = &nsh_io_default_print_prompt,
    .erase_last_char = &nsh_io_default_erase_last_char,
    .erase_line = &nsh_io_default_erase_line,
#if NSH_FEATURE_USE_PRINTF == 1
    .printf = &nsh_io_default_printf,
#endif
};
#endif

#ifdef __cplusplus
}
#endif

#endif // NSH_IO_PLUGIN_DEFAULT_H_
