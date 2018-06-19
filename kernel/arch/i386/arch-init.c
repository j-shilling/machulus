#include <kernel/panic.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>

#include "frames.h"
#include "paging.h"

#include <stdio.h>

extern void tty_init (void);

static frame_t * const frame_stack = (frame_t *) (0xB0000000);
static int sp = -1;

void
arch_init (uint32_t mmap_addr, uint32_t mmap_length)
{
  tty_init ();
  frame_init (mmap_addr, mmap_length);

  frame_t frame = frame_alloc ();
  int result;
  while (__frame_valid (frame))
    {
      sp++;
      if (0 == (sp % (PAGE_SIZE / sizeof (frame_t))))
        page_map (frame_stack + sp, frame);
      else
        frame_stack[sp] = frame;

      frame = frame_alloc ();
    }

  if (frame != (frame_t) ENOMEM)
    panic ("Got error code %d", (int) frame);

  while (sp >= 0)
    {
      if (0 == (sp % (PAGE_SIZE / sizeof (frame_t))))
        frame = page_unmap (frame_stack + sp);
      else if (sp > 0)
        frame = frame_stack[sp];
      sp --;
      
      result = frame_free (frame);
      if (result)
        panic ("Got error code %d", result);
    }
}
