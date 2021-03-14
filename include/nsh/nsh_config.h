/*
 * nsh_config.h
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#ifndef NSH_CONFIG_H_
#define NSH_CONFIG_H_

/******************************************************************************
 *** User configuration section
 ******************************************************************************/

/*
 * Maximum character count you can enter for a command (including arguments).
 * If you exceed this number, the read line function will return the status
 * NSH_STATUS_BUFFER_OVERFLOW, and a warning will be displayed.
 */
#define NSH_LINE_BUFFER_SIZE 128u

/*
 * Maximum character count for commands name and arguments string.
 * If you try to register a command with a name greater than this,
 * the registration function will return NSH_STATUS_WRONG_ARG.
 */
#define NSH_MAX_STRING_SIZE 16u

/*
 * Maximum number of command you can register in nsh.
 * If you reach this number, all registration request will be ignored and
 * the registration function will return NSH_STATUS_MAX_CMD_NB_REACH.
 */
#define NSH_CMD_MAX_COUNT 32u

/*
 * Maximum number of arguments you can write in a command line.
 * An argument is anything between whitespaces, ie "cmd arg1 arg2=true"
 * contains three arguments: "cmd", "arg1", and "arg2=true"
 * If you reach this number, the argument line split function will return with
 * the status NSH_STATUS_MAX_ARGS_NB_REACH.
 */
#define NSH_CMD_ARGS_MAX_COUNT 32u

/*
 * Maximum number of command memorized into the history.
 * If you exceed this number, oldest commands will be overwritten by recent
 * ones.
 * NB: this history takes NSH_CMD_HISTORY_SIZE*NSH_LINE_BUFFER_SIZE bytes
 * in BSS region, which can be pretty huge...
 * Require: NSH_FEATURE_USE_HISTORY == 1
 */
#define NSH_CMD_HISTORY_SIZE 16u

/*
 * Default prompt displayed at the beginning of each command line.
 */
#define NSH_DEFAULT_PROMPT "> "

/*
 * Allow command auto-completion using tabulation key.
 */
#define NSH_FEATURE_USE_AUTOCOMPLETION 1

/*
 * Allow command memorization and navigation through the history using up and
 * down arrows.
 */
#define NSH_FEATURE_USE_HISTORY 1

/*
 * Define a printf-like function, which can be resource hungry...
 */
#define NSH_FEATURE_USE_PRINTF 1

/*
 * Allow command return code printing (for debug purpose)
 * Require: NSH_FEATURE_USE_PRINTF == 1
 */
#define NSH_FEATURE_USE_RETURN_CODE_PRINTING 1

/******************************************************************************
 *** Internal configuration section (DO NOT TOUCH!)
 ******************************************************************************/

/*
 * Overwrite NSH_FEATURE_USE_RETURN_CODE_PRINTING to 0 if
 * NSH_FEATURE_USE_PRINTF == 0.
 */
#if NSH_FEATURE_USE_PRINTF == 0
#undef NSH_FEATURE_USE_RETURN_CODE_PRINTING
#define NSH_FEATURE_USE_RETURN_CODE_PRINTING 0
#endif

/*
 * Undef NSH_CMD_HISTORY_SIZE if NSH_FEATURE_USE_HISTORY == 0,
 * this symbol should not be used if the history is not used
 */
#if NSH_FEATURE_USE_HISTORY == 0
#undef NSH_CMD_HISTORY_SIZE
#endif

#endif /* NSH_CONFIG_H_ */
