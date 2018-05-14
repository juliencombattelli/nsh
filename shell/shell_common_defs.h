/*
 * shell_common_defs.h
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#ifndef SHELL_COMMON_DEFS_H_
#define SHELL_COMMON_DEFS_H_

#define SHELL_UNUSED(var) 		((void)var)

typedef enum shell_status
{
    SHELL_STATUS_OK,
    SHELL_STATUS_FAILURE,
    SHELL_STATUS_UNSUPPORTED,
    SHELL_STATUS_BUFFER_OVERFLOW,
    SHELL_STATUS_WRONG_ARG,
    SHELL_STATUS_EMPTY_CMD,
    SHELL_STATUS_CMD_NOT_FOUND,
    SHELL_STATUS_MAX_CMD_NB_REACH,
    SHELL_STATUS_MAX_ARGS_NB_REACH,

} shell_status_t;

#endif /* SHELL_COMMON_DEFS_H_ */
