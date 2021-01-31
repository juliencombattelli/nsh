/*
 * nsh_common_defs.h
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#ifndef NSH_COMMON_DEFS_H_
#define NSH_COMMON_DEFS_H_

#define NSH_UNUSED(var) ((void)var)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum nsh_status {
    NSH_STATUS_OK,
    NSH_STATUS_QUIT,
    NSH_STATUS_FAILURE,
    NSH_STATUS_UNSUPPORTED,
    NSH_STATUS_BUFFER_OVERFLOW,
    NSH_STATUS_WRONG_ARG,
    NSH_STATUS_EMPTY_CMD,
    NSH_STATUS_CMD_NOT_FOUND,
    NSH_STATUS_MAX_CMD_NB_REACH,
    NSH_STATUS_MAX_ARGS_NB_REACH,
} nsh_status_t;

#ifdef __cplusplus
}
#endif

#endif /* NSH_COMMON_DEFS_H_ */
