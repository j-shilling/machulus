#include <stddef.h>

#include <kernel/panic.h>

#include "paging.h"
#include "frames.h"
#include "multiboot.h"

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

int sp = -1;
frame_t *const frame_stack = (frame_t*)(FRAME_STACK_VBASE);

static inline int
__blocks_overlap (uint32_t x, size_t x_len, uint32_t y, size_t y_len)
{
  /* is x in y? */
  if ((x >= y) && (x < (y + y_len)))
    return 1;
  /* is y in x? */
  if ((y >= x) && (y < (x + x_len)))
    return 1;

  return 0;
}

void
frames_init (uint32_t mmap_addr, uint32_t mmap_length)
{
  if (0 == mmap_addr)
    panic ("Boot loader did not give us a memory map");

  uint32_t mmap_start = mmap_addr;
  uint32_t mmap_end   = (mmap_addr + mmap_length);
  uint32_t kstart     = (uint32_t)(&_kernel_start) - KERNEL_VBASE;
  uint32_t kend       = (uint32_t)(&_kernel_end)   - KERNEL_VBASE;

  /* Hold the frames used by the memory map itself */
  uint32_t tmp_frames[(mmap_length / PAGE_SIZE) + 1];
  size_t   n_tmp_frames = 0;

  while (mmap_addr < mmap_end)
    {
      multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mmap_addr;

      if ((MULTIBOOT_MEMORY_AVAILABLE == mmap->type) /* memory is available */
	  && (mmap->addr <= 0xFFFFFFFF))             /* memory is a 32-bit address */
	{
	  /* Get end of block */
	  uint32_t block_end = ((mmap->addr + mmap->len) < 0xFFFFFFFF) ?
	    (uint32_t)(mmap->addr + mmap->len) : 0xFFFFFFFF;
	  /* Get first frame aligned address */
	  uint32_t frame = (mmap->addr & 0x00000FFF) ?
	    (mmap->addr & 0xFFFFF000) + 0x1000 : mmap->addr;

	  while ((frame + PAGE_SIZE) < block_end)
	    {
	      /* ignore anything that overlaps the kernel */
	      if (!__blocks_overlap (frame, PAGE_SIZE, kstart, (kend - kstart)))
		{
		  /* we cannot free mmap frames while it is being read. save
		   * for later */
		  if (__blocks_overlap (frame, PAGE_SIZE, mmap_start, mmap_length))
		    {
		      tmp_frames[n_tmp_frames++] = frame;
		    }
		  else
		    {
		      frames_free (frame);
		    }
		}

	      frame += PAGE_SIZE;
	    }
	}

      mmap_addr += (mmap->size + sizeof (mmap->size));
    }

  /* we are no done with mmap and can free those frames */
  for (size_t i = 0; i < n_tmp_frames; i ++)
    frames_free (tmp_frames[i]);
}

void
frames_free (frame_t frame)
{
  frame &= 0xFFFFF000;

  sp ++;
  if (0 == (sp % 1024))
    {
      int ret = page_map (frame_stack + sp, frame);
      if (ret != 0)
        panic ("Could not map to frame stack (%p). Error code %d",
               frame_stack + sp, ret);
    }
  else
    {
      frame_stack[sp] = frame;
    }
}

uint32_t
frames_alloc (void)
{
  if (sp == -1)
    {
      return 1; /* 1 is an invalid frame */
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
