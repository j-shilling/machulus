#include <stddef.h>

extern uint32_t kernel_end;
static uint32_t placement_address = (uint32_t)(&kernel_end);

uint32_t
__dirty_malloc (size_t size, int align, uint32_t *phys)
{
  if (align && (placement_address & 0xFFFFF000))
    {
      placement_address &= 0xFFFFF000;
      placement_address += 0x1000;
    }
  if (phys)
    {
      *phys = placement_address;
    }

  uint32_t tmp = placement_address;
  placement_address += size;
  return tmp;
}
