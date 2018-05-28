#pragma once

#include <sys/cdefs.h>
#include <stdint.h>

__BEGIN_DECLS

void frames_init (uint32_t, uint32_t);
void frames_free (uint32_t);
uint32_t frames_alloc (void);

__END_DECLS
