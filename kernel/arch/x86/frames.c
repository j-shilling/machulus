#include <stddef.h>
#include <errno.h>

#include <kernel/panic.h>
#include <stdint.h>

#include "paging.h"
#include "frames.h"
#include "multiboot.h"

#define FRAME_MAX   (0xFFFFF000)
#define	  __page_align(x)  (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

static int sp = -1;
static frame_t * const frame_stack = (frame_t *) (FRAME_STACK_VBASE);

uintptr_t kernel_phys_end;

uintptr_t mmap_start;
uintptr_t mmap_addr;
uintptr_t mmap_end;

uintptr_t block_addr;
uintptr_t block_end;

static inline int
__next_block (void)
{
  while (mmap_addr < mmap_end)
    {
      multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mmap_addr;

      if ((MULTIBOOT_MEMORY_AVAILABLE == mmap->type)
          /* memory is available */
          && (mmap->addr <= FRAME_MAX))
        /* memory is addressable */
        {
          /* Set block_* vars */
          block_addr = (mmap->addr & (PAGE_SIZE - 1)) ?
                  ((mmap->addr & ~(PAGE_SIZE - 1)) + PAGE_SIZE) :
                  (mmap->addr);
          block_end = ((UINTPTR_MAX - mmap->len) < mmap->addr) ?
                  (UINTPTR_MAX) :
                  (mmap->addr + mmap->len);

          /* Check that this block is after the kernel */
          if (block_addr < kernel_phys_end)
            block_addr = (kernel_phys_end & ~(PAGE_SIZE - 1)) + PAGE_SIZE;

          /* Check that this block works for a frame */
          if ((block_end - PAGE_SIZE) >= block_addr)
            {
              mmap_addr += (mmap->size + sizeof (mmap->size));
              return 0;
            }
        }

      mmap_addr += (mmap->size + sizeof (mmap->size));
    }

  /* Out of memory, no next block */
  return 1;
}

void
frame_init (uint32_t _mmap_addr, uint32_t mmap_length)
{
  if (0 == _mmap_addr)
    panic ("Boot loader did not give us a memory map");

  kernel_phys_end = (uintptr_t) (&_kernel_end) - KERNEL_VBASE;

  mmap_addr = (uintptr_t) _mmap_addr;
  mmap_start = mmap_addr;
  mmap_end = mmap_addr + mmap_length;

  if (__next_block ())
    panic ("Could not find an initial free memory block");
}

int
frame_free (frame_t frame)
{
  if (!__frame_valid (frame))
    return EINVAL;

  sp++;
  if (0 == (sp % (PAGE_SIZE / sizeof (frame_t))))
    return page_map (frame_stack + sp, frame);
  else
    frame_stack[sp] = frame;

  return 0;
}

frame_t
frame_alloc (void)
{
  if (0 == (sp % (PAGE_SIZE / sizeof (frame_t))))
    return page_unmap (frame_stack + sp--);
  else if (sp > 0)
    return frame_stack[sp--];

  /* No freed frames; create on from current memory block */
  if ((block_end - PAGE_SIZE) < block_addr)
    {
      if (__next_block ())
        return (frame_t) ENOMEM;
    }

  frame_t ret = block_addr;
  block_addr += PAGE_SIZE;
  return ret;
}
