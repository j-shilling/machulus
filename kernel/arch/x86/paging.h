#pragma once

#define KERNEL_VBASE		(0xC0000000)
#define PHYS_ADDR(x)		((x) - KERNEL_VBASE)
#define VIRT_ADDR(x)            ((x) + KERNEL_VBASE)

#define FRAME_STACK_VBASE	(KERNEL_VBASE + 0x10000000)
#define KHEAP_VBASE		(KERNEL_VBASE + 0x20000000)

#define PAGE_SIZE		(0x1000)

#define PAGE_FLAG_READ_WRITE	(1 << 1) /* Page is read-write accessible */
#define PAGE_FLAG_PRESENT	(1 << 0) /* Page is present */
#define PAGE_FLAG_USER          (1 << 2) /* Page is available in user space */

#define KPAGE_FLAGS		(PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE)

#ifndef ASM_FILE

#include <sys/cdefs.h>
#include <stdint.h>
#include "frames.h"

__BEGIN_DECLS

int  paging_grow_heap (int);
void paging_shrink_heap (int);

int page_map (void *, frame_t);
frame_t page_unmap (void *);

__END_DECLS

#endif /* !ASM_FILE */
