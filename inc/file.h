#pragma once

#include <stdio.h>

struct FILE {
  size_t (*fwrite)(const void *, size_t, size_t, FILE *);
};
