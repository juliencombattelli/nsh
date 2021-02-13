/*
 * nsh_common_defs.h
 *
 *  Created on: May 10, 2018
 *      Author: jucom
 */

#ifndef NSH_COMMON_DEFS_H_
#define NSH_COMMON_DEFS_H_

#define NSH_UNUSED(var) ((void)var)

/** 
 * @def NON_NULL(<arg-index>...)
 * @brief Indicates that listed pointer arguments shall not be null.
 * 
 * This macro acts as a precondition for a function, indicating that arguments
 * whose index is present in the list must not be null. The precondition is meant
 * to be checkable by a compiler (GCC and Clang at least).
 * If a null pointer is passed to an argument marked as NON_NULL, and the function
 * does not check if this argument is null, then the behaviour is undefined.
 * 
 * @example
 * // When calling func, i and c arguments shall not be null
 * void func(int* i, float f, char* c, void* p) NON_NULL(1,3)
 * 
 */
#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
#define NON_NULL(...) __attribute__((nonnull(__VA_ARGS__)))
#else
#define NON_NULL(...)
#endif

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
