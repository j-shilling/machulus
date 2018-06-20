#pragma once

#include <sys/cdefs.h>
#include <stdint.h>

__BEGIN_DECLS

typedef uint32_t frame_t;

void frame_init (uint32_t, uint32_t);
int frame_free (frame_t);
frame_t frame_alloc (void);

static inline int
__frame_valid (frame_t frame)
{
    return !(frame & 0xFFF);
}

__END_DECLS
