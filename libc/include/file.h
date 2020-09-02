/**
 * @file file.h
 * @brief Implements FILE for use in kernel space.
 */

#pragma once

#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Implementation of FILE.
 *
 * @details The C standard library uses an opaque FILE object to represent an
 * I/O stream. This structure provides a stream implementation to work
 * with the I/O functions which run in kernel-space.
 *
 * @see stdio.h
 */
struct FILE {
  /**
   * @brief References an implementation of fwrite.
   *
   * @see fwrite
   */
  size_t (*fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);

  /**
   * @brief A spinlock to provide synchronized access to this stream.
   */
  volatile bool lock;
};

static inline void __lock_file(FILE *file) {
  while(!__atomic_test_and_set(&file->lock, __ATOMIC_ACQUIRE));
}

static inline void __unlock_file(FILE *file) {
  __atomic_clear(&file->lock, __ATOMIC_RELEASE);
}
