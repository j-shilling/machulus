#pragma once

#include <stdio.h>
#include <stdbool.h>

struct FILE {
  size_t (*fwrite)(const void *, size_t, size_t, FILE *);

  volatile bool lock;
};

static inline void __lock_file(FILE *file) {
  while(!__atomic_test_and_set(&file->lock, __ATOMIC_ACQUIRE));
}

static inline void __unlock_file(FILE *file) {
  __atomic_clear(&file->lock, __ATOMIC_RELEASE);
}
