/*
 * nsh_history.c
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#include <nsh/nsh_config.h>

#if NSH_FEATURE_USE_HISTORY == 1

#include <nsh/nsh_common_defs.h>
#include <nsh/nsh_history.h>
#include <string.h>

void nsh_history_reset(nsh_history_t* hist)
{
    hist->head = 0;
    hist->tail = 0;
    hist->size = 0;
}

unsigned int nsh_history_entry_count(const nsh_history_t* hist)
{
    return hist->size;
}

bool nsh_history_is_full(const nsh_history_t* hist)
{
    return (hist->size == NSH_CMD_HISTORY_SIZE);
}

bool nsh_history_is_empty(const nsh_history_t* hist)
{
    return (hist->size == 0);
}

void nsh_history_add_entry(nsh_history_t* hist, const char* entry)
{
    strncpy(hist->entries[hist->head], entry, sizeof(hist->entries[hist->head]));

    hist->head = (hist->head + 1) % NSH_CMD_HISTORY_SIZE;

    if (nsh_history_is_full(hist)) {
        hist->tail = (hist->tail + 1) % NSH_CMD_HISTORY_SIZE;
    } else {
        hist->size++;
    }
}

int nsh_history_get_entry(nsh_history_t* hist, unsigned int age, char* entry)
{
    if (age >= nsh_history_entry_count(hist)) {
        return NSH_STATUS_WRONG_ARG;
    }

    unsigned int most_recent_entry = hist->head - 1; // head points to the element just after the most recent entry
    unsigned int entry_index = most_recent_entry - age;
    if (entry_index > NSH_CMD_HISTORY_SIZE) {
        entry_index += NSH_CMD_HISTORY_SIZE;
    }

    strncpy(entry, hist->entries[entry_index], NSH_LINE_BUFFER_SIZE);

    return NSH_STATUS_OK;
}

#endif /* NSH_FEATURE_USE_HISTORY == 1 */
