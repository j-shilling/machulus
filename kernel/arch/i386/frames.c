#include <stddef.h>

#include <kernel/panic.h>
#include <stdint.h>

#include "paging.h"
#include "frames.h"
#include "multiboot.h"

#define	  __page_align(x)  (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

uintptr_t kernel_phys_start;
uintptr_t kernel_phys_end;

uintptr_t mmap_addr;
uintptr_t mmap_end;

uintptr_t block_addr;
uintptr_t block_end;

int sp = -1;
uintptr_t *frame_stack = (uintptr_t *)(FRAME_STACK_VBASE);

static int
__next_block (void)
{
  while (mmap_addr < mmap_end)
    {
      multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mmap_addr;

      if ((MULTIBOOT_MEMORY_AVAILABLE == mmap->type) /* memory is available */
          && (mmap->addr > kernel_phys_end)          /* memory is after the kernel */
	  && (mmap->addr <= UINTPTR_MAX))            /* memory is addressable */
	{
          block_addr = (uintptr_t) __page_align(mmap->addr);
          if ((mmap->len > UINTPTR_MAX) || ((UINTPTR_MAX - mmap->len) < mmap->addr))
            block_end = UINTPTR_MAX;
          else
            block_end = block_addr + mmap->len;
          
          return 0;
        }

      mmap_addr += (mmap->size + sizeof (mmap->size));
    }
  
  return 1;
}

void
frames_init (uint32_t _mmap_addr, uint32_t _mmap_length)
{
  kernel_phys_start = (uintptr_t)(&_kernel_start) - KERNEL_VBASE;
  kernel_phys_end   = (uintptr_t)(&_kernel_end) - KERNEL_VBASE;
  
  if (0 == _mmap_addr)
    panic ("Boot loader did not give us a memory map");
  if (mmap_addr > kernel_phys_start)
    panic ("Boot loader put the memory map after the kernel");
  
  mmap_addr = (uintptr_t) _mmap_addr;
  mmap_end  = mmap_addr + _mmap_length;

  __next_block();
}

void
frames_free (uintptr_t frame)
{
  frame &= 0xFFFFF000;

  sp ++;
  if (0 == (sp % 1024))
    {
      paging_map_frame_stack ((uint32_t)(frame_stack + sp), frame);
    }
  else
    {
      frame_stack[sp] = frame;
    }
}

uintptr_t
frames_alloc (void)
{
  if (sp == -1)
    {
      while ((block_end - PAGE_SIZE) < block_addr)
        {
          if (__next_block())
            return 1; /* Out of memory */
        }
      
      uintptr_t ret = block_addr;
      block_addr += PAGE_SIZE;
      return ret;
    }
  if (0 == (sp % 1024))
    {
      return paging_unmap_frame_stack ((uint32_t)(frame_stack + sp));
    }
  else
    {
      return frame_stack[sp--];
    }
}
