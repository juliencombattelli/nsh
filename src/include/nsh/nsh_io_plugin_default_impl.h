#ifndef NSH_IO_PLUGIN_DEFAULT_IMPL_H_
#define NSH_IO_PLUGIN_DEFAULT_IMPL_H_

#include <nsh/nsh_config.h>

#ifdef __cplusplus
extern "C" {
#endif

char nsh_io_get_char(void);

void nsh_io_put_char(char c);

void nsh_io_put_newline(void);

void nsh_io_put_string(const char* str);

void nsh_io_put_buffer(const char* str, unsigned int size);

void nsh_io_print_prompt(void);

void nsh_io_erase_last_char(void);

void nsh_io_erase_line(void);

#if NSH_FEATURE_USE_PRINTF == 1
int nsh_io_printf(const char* NSH_RESTRICT format, ...);
#endif

#ifdef __cplusplus
}
#endif

#endif // NSH_IO_PLUGIN_DEFAULT_IMPL_H_
