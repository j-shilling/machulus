#pragma once

#define KERNEL_OFFSET 0xffffffff80000000
#define PAGE_SIZE 0x1000

#define PAGE_PRESENT (1 << 0)
#define PAGE_WRITABLE (1 << 1)

#define virtual_to_physical(addr) (addr - KERNEL_OFFSET)
#define physical_to_virtual(addr) (addr + KERNEL_OFFSET)
