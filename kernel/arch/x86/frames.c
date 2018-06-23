#include <stddef.h>
#include <errno.h>

#include <kernel/panic.h>
#include <stdint.h>

#include "paging.h"
#include "frames.h"
#include "multiboot.h"

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

unsigned char frame_map[N_FRAMES / 8] = { 0 };

static unsigned int
__frame_to_index (frame_t frame)
{
  return (frame / FRAME_SIZE);
}

static frame_t
__index_to_frame (size_t index)
{
  return (frame_t)(FRAME_SIZE * index);
}

static frame_t
__phys_addr_to_frame (phys_addr_t addr)
{
  return (addr & FRAME_T_MAX);
}

static void
__frame_set_available (phys_addr_t addr, size_t length)
{
  frame_t first_frame = __phys_addr_to_frame (addr);
  frame_t last_frame  = __phys_addr_to_frame (addr + length - 1);
  
  unsigned int start = __frame_to_index (first_frame);
  unsigned int end   = __frame_to_index (last_frame);
  
  unsigned int *__frame_map = (unsigned int *) frame_map;
  size_t bits_per_int = sizeof (unsigned int) * 8;
  
  size_t index = start / bits_per_int;
  for (int i = 0; i < (start % bits_per_int); i ++)
    __frame_map[index] |= (1 << i);
  
  for (; index < (end / bits_per_int); index ++)
    __frame_map[index] = ~(0);
  
  for (int i = 0; i < (end % bits_per_int); i ++)
    __frame_map[index] |= (1 << i);
}

static void
__frame_set_used (phys_addr_t addr, size_t length)
{
  frame_t first_frame = __phys_addr_to_frame (addr);
  frame_t last_frame  = __phys_addr_to_frame (addr + length - 1);
  
  unsigned int start = __frame_to_index (first_frame);
  unsigned int end   = __frame_to_index (last_frame);
  
  unsigned int *__frame_map = (unsigned int *) frame_map;
  size_t bits_per_int = sizeof (unsigned int) * 8;
  
  size_t index = start / bits_per_int;
  for (int i = 0; i < (start % bits_per_int); i ++)
    __frame_map[index] &= ~(1 << i);
  
  for (; index < (end / bits_per_int); index ++)
    __frame_map[index] = 0;
  
  for (int i = 0; i < (end % bits_per_int); i ++)
    __frame_map[index] &= ~(1 << i);
}

void
frame_init (uint32_t _mmap_addr, uint32_t mmap_length)
{
  if (0 == _mmap_addr)
    panic ("Boot loader did not give us a memory map");
  
  uintptr_t mmap_addr = (uintptr_t) _mmap_addr;
  uintptr_t mmap_end = mmap_addr + mmap_length;
  
  while (mmap_addr < mmap_end)
    {
      multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mmap_addr;

      phys_addr_t addr = (phys_addr_t) mmap->addr;
      size_t len = (size_t) mmap->len;
      
      if (MULTIBOOT_MEMORY_AVAILABLE == mmap->type)
        __frame_set_available (addr, len);

      mmap_addr += (mmap->size + sizeof (mmap->size));
    }

  __frame_set_used ((phys_addr_t)(&_kernel_start) - KERNEL_VBASE,
                  (uintptr_t)(&_kernel_end) - (uintptr_t)(&_kernel_start));
}

int
frame_free (frame_t frame)
{
  if (!__frame_valid (frame))
    return EINVAL;

  size_t index = __frame_to_index (frame);
  frame_map[index / 8] |= (1 << (index % 8));

  return 0;
}

frame_t
frame_alloc (void)
{
  unsigned int *__frame_map = (unsigned int *)frame_map;
  size_t bits_per_int = sizeof (unsigned int) * 8;
  size_t frame_map_size = sizeof (frame_map) / sizeof (unsigned int);
  
  for (size_t i = 0; i < frame_map_size; i ++)
    {
      if (__frame_map[i])
        {
          for (size_t j = 0; j < bits_per_int; j ++)
            {
              if (__frame_map[i] & (1 << j))
                {
                  return __index_to_frame ((i * bits_per_int) + j);
                }
            }
        }
    }
  
  return (frame_t) ENOMEM;
}