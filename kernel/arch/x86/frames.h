#pragma once

#include <sys/cdefs.h>
#include <stdint.h>
#include <stddef.h>

__BEGIN_DECLS

#define SIZEOF_FRAME_T (4)
#define FRAME_SIZE (0x1000)
#define FRAME_T_MAX (0xFFFFF000)
#define N_FRAMES (FRAME_T_MAX / FRAME_SIZE)

typedef uint32_t frame_t;
typedef uint32_t phys_addr_t;

void frame_init(uint32_t, uint32_t);
void frame_set_available(phys_addr_t, size_t);
void frame_set_used(phys_addr_t, size_t);

int frame_free(frame_t);
frame_t frame_alloc(void);

static inline int __frame_valid(frame_t frame)
{
    return !(frame & 0xFFF);
}

__END_DECLS
