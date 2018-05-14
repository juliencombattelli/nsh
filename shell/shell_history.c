/*
 * shell_history.c
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#include "shell_config.h"

#if SHELL_FEATURE_USE_HISTORY == 1

#include "shell_history.h"
#include "shell_common_defs.h"
#include <string.h>

void shell_history_reset(shell_history_t *hist)
{
    hist->head = 0;
    hist->tail = 0;
    hist->size = 0;
}

int shell_history_entry_count(const shell_history_t *hist)
{
    return hist->size;
}

bool shell_history_is_full(const shell_history_t *hist)
{
    return (hist->size == SHELL_CMD_HISTORY_SIZE);
}

bool shell_history_is_empty(const shell_history_t *hist)
{
    return (hist->size == 0);
}

int shell_history_add_entry(shell_history_t *hist, const char *entry)
{
    if (!hist)
        return SHELL_STATUS_WRONG_ARG;

    strcpy(hist->entries[hist->head], entry);

    hist->head = (hist->head + 1) % SHELL_CMD_HISTORY_SIZE;

    if(shell_history_is_full(hist))
        hist->tail = (hist->tail + 1) % SHELL_CMD_HISTORY_SIZE;
    else
        hist->size++;

    return SHELL_STATUS_OK;
}

int shell_history_get_entry(shell_history_t *hist, int age, char *entry)
{
    if (!hist || !entry || age < 0 || age >= shell_history_entry_count(hist))
        return SHELL_STATUS_WRONG_ARG;

    int most_recent_entry = hist->head-1; // head points to the element just after the most recent entry
    int entry_index = most_recent_entry-age;
    if (entry_index < 0)
        entry_index += SHELL_CMD_HISTORY_SIZE;

    strcpy(entry, hist->entries[entry_index]);

    return SHELL_STATUS_OK;
}

#endif
