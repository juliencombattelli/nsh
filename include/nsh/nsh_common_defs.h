#ifndef NSH_COMMON_DEFS_H_
#define NSH_COMMON_DEFS_H_

#include <nsh/nsh_version.h>

/**
 * @def NSH_TO_STRING(<token>)
 * @brief Convert <token> into a string, expanding macro if needed.
 */
#define NSH_STRINGIFY_(x) #x
#define NSH_TO_STRING(x)  NSH_STRINGIFY_(x)

/**
 * @def NSH_UNUSED(<var-name>)
 * @brief Indicates that <var-name> is unused in the current scope.
 *
 * This macro is used to prevent some compiler warnings about unused variables.
 */
#define NSH_UNUSED(var) ((void)var)

/**
 * @def NSH_NON_NULL(<arg-index>...)
 * @brief Indicates that listed pointer arguments shall not be null.
 *
 * This macro acts as a precondition for a function, indicating that arguments
 * whose index is present in the list must not be null. The precondition is meant
 * to be checkable by a compiler (GCC and Clang at least).
 * If a null pointer is passed to an argument marked as NSH_NON_NULL, and the function
 * does not check if this argument is null, then the behaviour is undefined.
 *
 * @example
 * // When calling func, i and c arguments shall not be null
 * void func(int* i, float f, char* c, void* p) NSH_NON_NULL(1,3)
 *
 * @note MSVC also implements something similar but the usage is not compatible with
 * GCC and Clang.
 */
#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
#define NSH_NON_NULL(...) __attribute__((nonnull(__VA_ARGS__)))
#else
#define NSH_NON_NULL(...)
#endif

/**
 * @def NSH_RESTRICT
 * @brief Portable restrict keyword for both C and C++.
 *
 * This macro allows the usage of restrict keyword when nsh is compiled as C, and
 * the usage of corresponding compilers extension when compiled as C++.
 * If a compiler does not provide an alternative restrict keyword for C++, this
 * macro expands to nothing.
 */
#ifdef __cplusplus
#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
#define NSH_RESTRICT __restrict__
#elif defined(_MSC_VER)
#define NSH_RESTRICT __restrict
#else
#define NSH_RESTRICT /* empty */
#endif
#else
#define NSH_RESTRICT restrict
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

#endif // NSH_COMMON_DEFS_H_
