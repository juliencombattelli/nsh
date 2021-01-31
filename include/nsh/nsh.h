/*
 * nsh.h
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#ifndef NSH_H_
#define NSH_H_

#include <nsh/nsh_cmd.h>

#ifdef __cplusplus
extern "C" {
#endif

int nsh_init();

int nsh_register_command(const char* name, nsh_cmd_handler_t handler);

void nsh_run(void);

#ifdef __cplusplus
}
#endif

#endif /* NSH_H_ */
