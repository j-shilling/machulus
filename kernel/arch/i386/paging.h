#pragma once

#define KERNEL_VBASE		(0xC0000000)
#define PAGE_SIZE		(0x1000)

#define PAGE_FLAG_READ_WRITE	(1 << 1) /* Page is read-write accessible */
#define PAGE_FLAG_PRESENT	(1 << 0) /* Page is present */

#ifndef ASM_FILE

#include <stdint.h>

extern uint32_t kernel_page_directory[1024];
extern uint32_t kernel_page_table[1024];

#endif /* !ASM_FILE */
