/**
 * @file errno.h
 *
 * @brief Provides the symbol @c errno and various macros to represent
 * specific error codes.
 */

#pragma once

/**
 * @brief an integer variable set by some functions to indicate an
 * error.
 *
 * @details Kernel implementations of standard library functions will
 * try to follow the C standard as closely as possible and will set
 * the value of @c errno under the same conditions that their @c libc
 * equivalents would.
 *
 * @todo Make this thread-safe
 */
#define errno (*__errno_address())

/**
 * @brief Returns an address of @c errno.
 *
 * @details This will be needed to implemented @c errno in a
 * thread-safe way, but it should not be used except by the macro @c
 * errno.
 *
 * @see errno
 */
inline int *__errno_address(void);
