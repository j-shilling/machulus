#include "multiboot.h"
#include "paging.h"
#include "frames.h"

#include <stdint.h>
#include <stddef.h>

uint32_t kernel_page_directory[1024] __attribute__ ((aligned (PAGE_SIZE))) = { 0 };
uint32_t kernel_page_table[1024]     __attribute__ ((aligned (PAGE_SIZE))) = { 0 };
uint32_t frames_page_table[1024]     __attribute__ ((aligned (PAGE_SIZE))) = { 0 };

uint32_t kheap_page_table[1024]      __attribute__ ((aligned (PAGE_SIZE))) = { 0 };
size_t   kheap_index = 0;

void
paging_map_frame_stack (uint32_t vaddr, uint32_t paddr)
{
  paddr &= 0xFFFFF000;
  uint32_t index = ((vaddr & 0x003FF000) >> 12);
  frames_page_table[index] = (paddr | KPAGE_FLAGS);
}

uint32_t
paging_unmap_frame_stack (uint32_t vaddr)
{
  uint32_t index = ((vaddr & 0x003FF000) >> 12);
  uint32_t ret = frames_page_table[index];
  frames_page_table[index] = 0;
  return (ret & 0xFFFFF000);
}

int
paging_grow_heap (int nframes)
{
  /* Don't deal with neg numbs */
  if (nframes < 0)
    return -1;

  /* Don't do anything for 0 frames */
  if (nframes == 0)
    return 0;

  /* Don't grow the heap bigger than 1 page table */
  if ((kheap_index + nframes) >= 1024)
    return -1;

  /* Go for it */
  size_t index = kheap_index;
  for (int i = 0; i < nframes; i ++)
    {
      uint32_t frame = frames_alloc ();
      if (frame & 0x00000FFF)
	/* We are out of frames */
	goto frame_error;
      kheap_page_table[index++] = (frame | KPAGE_FLAGS);
    }

  kheap_index = index;
  return 0;

frame_error:
  /* Undo whatever was done before the frame error */
  for (; index >= kheap_index; index --)
    {
      uint32_t frame = (kheap_page_table[index] & 0xFFFFF000);
      frames_free (frame);
      kheap_page_table[index--] = 0;
    }
  return -1;
}

void
paging_shrink_heap (int nframes)
{
  if (nframes >= 0)
    return;

  for (int i = 0; (i < nframes) && (kheap_index >= 0); i ++)
    {
      uint32_t frame = (kheap_page_table[kheap_index] & 0xFFFFF000);
      frames_free (frame);
      kheap_page_table[kheap_index --] = 0;
    }
}
