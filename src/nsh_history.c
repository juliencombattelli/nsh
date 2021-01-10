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

int nsh_history_entry_count(const nsh_history_t* hist)
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

int nsh_history_add_entry(nsh_history_t* hist, const char* entry)
{
    if (!hist) {
        return NSH_STATUS_WRONG_ARG;
    }

    strncpy(hist->entries[hist->head], entry, sizeof(hist->entries[hist->head]));

    hist->head = (hist->head + 1) % (int)NSH_CMD_HISTORY_SIZE;

    if (nsh_history_is_full(hist)) {
        hist->tail = (hist->tail + 1) % (int)NSH_CMD_HISTORY_SIZE;
    } else {
        hist->size++;
    }

    return NSH_STATUS_OK;
}

int nsh_history_get_entry(nsh_history_t* hist, int age, char* entry)
{
    if (!hist || !entry || age < 0 || age >= nsh_history_entry_count(hist)) {
        return NSH_STATUS_WRONG_ARG;
    }

    int most_recent_entry = hist->head - 1; // head points to the element just after the most recent entry
    int entry_index = most_recent_entry - age;
    if (entry_index < 0) {
        entry_index += NSH_CMD_HISTORY_SIZE;
    }

    strncpy(entry, hist->entries[entry_index], NSH_LINE_BUFFER_SIZE);

    return NSH_STATUS_OK;
}

#endif /* NSH_FEATURE_USE_HISTORY == 1 */
