#include <unistd.h>
#include <stdint.h>

#include "paging.h"

static void *const kheap_base = (void *)KHEAP_VBASE;
static char *kheap_max = (char *)KHEAP_VBASE;
static char *kheap_top = (char *)KHEAP_VBASE;

/* Sets the end of the heap to the value specified by addr,
   when that value is reasonable. On success returns 1, and
   returns -1 otherwise. */
int
brk (void *addr)
{
  /* Do we need to grow */
  if ((char*)addr > kheap_max)
    {
      /* Figure out how many frames we need to get to addr */
      intptr_t needed = (char *)addr - kheap_max;
      int nframes = (int)(needed / PAGE_SIZE) + 1;

      /* Grow the heap */
      if (paging_grow_heap (nframes))
	return -1;

      kheap_max += PAGE_SIZE * nframes;
    }

  /* Do we need to strink */
  else if ((char*)addr < (kheap_max - PAGE_SIZE))
    {
      intptr_t extra = kheap_max - (char *)addr;
      int nframes = (int)(extra / PAGE_SIZE);
      paging_shrink_heap (nframes);
      kheap_max -= PAGE_SIZE * nframes;
    }
  
  kheap_top = addr;
  return 0;
}

/* Increments the heap by increment bytes. If increment is
   0 then the current top of the heap is returned. On success,
   return the previous top of the heap, otherwise return (void *) -1 */
void *
sbrk (intptr_t increment)
{
  if (0 == increment)
    return (void *)kheap_top;

  void *old = (void*)kheap_top;
  if (brk (kheap_top + increment))
    return (void *)-1;
  return old;
}
