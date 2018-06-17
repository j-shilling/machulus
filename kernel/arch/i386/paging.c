#include "multiboot.h"
#include "paging.h"
#include "frames.h"

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

uint32_t kernel_page_directory[1024] __attribute__ ((aligned (PAGE_SIZE))) = {0};
uint32_t kernel_page_table[1024] __attribute__ ((aligned (PAGE_SIZE))) = {0};
uint32_t frames_page_table[1024] __attribute__ ((aligned (PAGE_SIZE))) = {0};

uint32_t kheap_page_table[1024] __attribute__ ((aligned (PAGE_SIZE))) = {0};
size_t kheap_index = 0;

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
  for (int i = 0; i < nframes; i++)
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
  for (; index >= kheap_index; index--)
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

  for (int i = 0; (i < nframes) && (kheap_index >= 0); i++)
    {
      uint32_t frame = (kheap_page_table[kheap_index] & 0xFFFFF000);
      frames_free (frame);
      kheap_page_table[kheap_index--] = 0;
    }
}

#include <errno.h>

typedef struct
{
  char bytes[1024];
} page_directory_t;

typedef struct
{
  char bytes[1024];
} page_table_t;

static inline page_directory_t *
__get_page_directory (uintptr_t __attribute__ ((unused)) addr)
{
  return (page_directory_t *) 0xFFFFF000;
}

static inline int
__table_index (uintptr_t addr)
{
  return ((addr >> 22) & 0x3FF);
}

static inline int
__page_index (uintptr_t addr)
{
  return ((addr >> 12) & 0x3FF);
}

static inline int
__page_directory_table_is_present (page_directory_t *page_directory, int index)
{
  return (((uint32_t *) page_directory)[index] & PAGE_FLAG_PRESENT);
}

static inline page_table_t *
__page_directory_get_table (page_directory_t * __attribute__ ((unused)) page_directory, int index)
{
  return (page_table_t *) (0xFFC00000 | (index << 12));
}

static inline void
__page_directory_set_table (page_directory_t *page_directory, int index, page_table_t *table, int flags)
{
  ((uint32_t *) page_directory)[index] = ((uint32_t) table) | flags;
}

static inline int
__page_table_page_is_present (page_table_t *table, int index)
{
  return (((uint32_t *) table)[index] & PAGE_FLAG_PRESENT);
}

static inline frame_t
__page_table_get_page (page_table_t *table, int index)
{
  return (((uint32_t *) table)[index] & 0xFFFFF000);
}

static inline void
__page_table_set_page (page_table_t *table, int index, frame_t frame, int flags)
{
  ((uint32_t *) table)[index] = ((uint32_t) frame) | flags;
}

static inline void
__tlb_flush_page (void *vaddr)
{
  asm volatile ("invlpg (%0)" :: "r" (vaddr) : "memory");
}

/**
 * Add virtual address mapping of a given physical address.
 * 
 * @param vaddr         Address to map to
 * @param phys          Address to be mapped
 * @param flags         Page flags
 * @return              0 on success; ENOMEM if there is not enough memory left
 *                      to create the needed structures; EADDRINUSE if vaddr is
 *                      already mapped. EINVAL if phys is not a valid frame.
 */
int
page_map (void *vaddr, frame_t phys)
{
  if (!__frame_valid (phys))
    return EINVAL;
  
  /* Set flags */
  unsigned int flags;
  if (((uintptr_t)vaddr) < KERNEL_VBASE)
    flags = PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE;
  else
    flags = PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE | PAGE_FLAG_USER;
  
  page_directory_t *page_directory = __get_page_directory ((uintptr_t) vaddr);

  /* Find location in page structures */
  int table_index = __table_index ((uintptr_t) vaddr);
  int page_index = __page_index ((uintptr_t) vaddr);

  /* Get the page table */
  if (!__page_directory_table_is_present (page_directory, table_index))
    {
      /* A new table is needed */
      frame_t frame = frames_alloc ();
      if (frame == (frame_t) ENOMEM)
        return ENOMEM;

      __page_directory_set_table (page_directory, table_index,
                                  (page_table_t *) frame, KPAGE_FLAGS);

      /* Initialize */
      char *bytes = ((char *) __page_directory_get_table (page_directory, table_index));
      for (int i = 0; i < sizeof (page_table_t); i++)
        bytes[i] = 0;
    }

  page_table_t *table = __page_directory_get_table (page_directory, table_index);

  /* Check address is available */
  if (__page_table_page_is_present (table, page_index))
    return EADDRINUSE;

  /* Set frame */
  __page_table_set_page (table, page_index, phys, flags);
  return 0;
}

/**
 * Remove a virtual address mapping.
 * 
 * @param vaddr         A page aligned virtual address
 * @return              On success returns the frame_t of the removed frame. If
 *                      vaddr was not page aligned returns EINVAL. If vaddr was
 *                      not previously mapped returns EADDRNOTAVAIL.
 */
frame_t
page_unmap (void *vaddr)
{
  /* Only work with page-aligned addresses */
  if (((uintptr_t)vaddr) & 0xFFF)
    return (frame_t) EINVAL;
  
  page_directory_t *page_directory = __get_page_directory ((uintptr_t) vaddr);

  /* Find location in page structures */
  int table_index = __table_index ((uintptr_t) vaddr);
  int page_index = __page_index ((uintptr_t) vaddr);
  
  /* Get the page table */
  if (!__page_directory_table_is_present (page_directory, table_index))
    return (frame_t) EADDRNOTAVAIL;
  page_table_t *table = __page_directory_get_table (page_directory, table_index);
  
  if (!__page_table_page_is_present (table, page_index))
    return (frame_t) EADDRNOTAVAIL;
  
  /* Get currently mapped frame */
  frame_t frame = __page_table_get_page (table, page_index);
  
  __page_table_set_page (table, page_index, (frame_t) 0, 0);
  __tlb_flush_page (vaddr);
  
  return frame;
}