#ifndef SHELL_IO_H_
#define SHELL_IO_H_

char shell_io_get_char(void);

void shell_io_put_char(char c);

void shell_io_put_newline(void);

void shell_io_put_string(const char *str);

void shell_io_put_buffer(const char *str, int size);

void shell_io_print_prompt(void);

void shell_io_erase_last_char(void);

void shell_io_erase_line(void);

#endif /* SHELL_IO_H_ */
