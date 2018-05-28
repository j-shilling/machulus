#pragma once

#define KERNEL_VBASE		(0xC0000000)
#define PHYS_ADDR(x)		((x) - KERNEL_VBASE)

#define FRAME_STACK_VBASE	(KERNEL_VBASE + 0x10000000)
#define KSTACK_VBASE		(KERNEL_VBASE + 0x20000000)

#define PAGE_SIZE		(0x1000)

#define PAGE_FLAG_READ_WRITE	(1 << 1) /* Page is read-write accessible */
#define PAGE_FLAG_PRESENT	(1 << 0) /* Page is present */

#define KPAGE_FLAGS		(PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE)

#ifndef ASM_FILE

#include <sys/cdefs.h>
#include <stdint.h>

__BEGIN_DECLS

void paging_map_frame_stack (uint32_t, uint32_t);
uint32_t paging_unmap_frame_stack (uint32_t);

__END_DECLS

#endif /* !ASM_FILE */
