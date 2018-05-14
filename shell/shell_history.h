/*
 * shell_history.h
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#ifndef SHELL_HISTORY_H_
#define SHELL_HISTORY_H_

#include "shell_config.h"

#if SHELL_FEATURE_USE_HISTORY == 1

#include <stdbool.h>

typedef struct shell_history
{
    char entries[SHELL_CMD_HISTORY_SIZE][SHELL_LINE_BUFFER_SIZE];
    int head;
    int tail;
    int size;

} shell_history_t;

void shell_history_reset(shell_history_t *hist);

int shell_history_entry_count(const shell_history_t *hist);

bool shell_history_is_full(const shell_history_t *hist);

bool shell_history_is_empty(const shell_history_t *hist);

int shell_history_add_entry(shell_history_t *hist, const char *entry);

int shell_history_get_entry(shell_history_t *hist, int age, char *entry);

#endif

#endif /* SHELL_HISTORY_H_ */
