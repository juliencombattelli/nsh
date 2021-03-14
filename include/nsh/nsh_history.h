/*
 * nsh_history.h
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#ifndef NSH_HISTORY_H_
#define NSH_HISTORY_H_

#include <nsh/nsh_common_defs.h>
#include <nsh/nsh_config.h>

#if NSH_FEATURE_USE_HISTORY == 1

#include <limits.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NSH_HISTORY_INVALID_ENTRY UINT_MAX

typedef struct nsh_history {
    char entries[NSH_CMD_HISTORY_SIZE][NSH_LINE_BUFFER_SIZE];
    unsigned int head; ///< Insertion index for new element
    unsigned int tail; ///< Oldest element index (0 if empty)
    unsigned int size;
} nsh_history_t;

void nsh_history_reset(nsh_history_t* hist) NON_NULL(1);

unsigned int nsh_history_entry_count(const nsh_history_t* hist) NON_NULL(1);

bool nsh_history_is_full(const nsh_history_t* hist) NON_NULL(1);

bool nsh_history_is_empty(const nsh_history_t* hist) NON_NULL(1);

void nsh_history_add_entry(nsh_history_t* hist, const char* entry) NON_NULL(1, 2);

int nsh_history_get_entry(nsh_history_t* hist, unsigned int age, char* entry) NON_NULL(1, 3);

#ifdef __cplusplus
}
#endif

#endif /* NSH_FEATURE_USE_HISTORY == 1 */

#endif /* NSH_HISTORY_H_ */
