/*
 * shell.h
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#ifndef SHELL_H_
#define SHELL_H_

#include <nsh/shell_cmd.h>

int shell_init();

int shell_register_command(const char* name, shell_cmd_handler_t handler);

void run_shell(void);

#endif /* SHELL_H_ */
