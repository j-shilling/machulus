#pragma once

#include <sys/cdefs.h>
#include <stdint.h>

__BEGIN_DECLS

typedef uint32_t frame_t;

void frames_init (uint32_t, uint32_t);
void frames_free (uint32_t);
uint32_t frames_alloc (void);

static inline int
__frame_valid (frame_t frame)
{
    return !(frame & 0xFFF);
}

__END_DECLS
