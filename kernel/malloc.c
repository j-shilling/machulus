#include <stdlib.h>
#include <unistd.h>

#define	  ALIGNMENT   8
#define	  __align(x)  (((x) + ALIGNMENT - 1) & ~(ALIGNMENT - 1))

typedef struct block_header_t
{
  size_t size;
  struct block_header_t *prev;
  struct block_header_t *next;
} block_header_t;

static const size_t header_size = __align (sizeof (block_header_t));

static inline block_header_t *
__block_header (void *block)
{
  return (block_header_t *)block - 1;
}

static inline void *
__block_addr (block_header_t *header)
{
  return (void *)(header + 1);
}

void *
malloc (size_t size)
{
  static block_header_t *heap_start = NULL;
  static block_header_t *heap_end   = NULL;
  
  size_t size_aligned = __align (size);
  block_header_t *block = heap_start;

  while (NULL != block)
    {
      if (!(block->size & 1) && (block->size >= size_aligned))
        {
          /* Check if block is worth breaking */
          if ((block->size - size_aligned - header_size) > ALIGNMENT)
            {
              block_header_t *next = 
                (block_header_t *)((char *)__block_addr (block) + size_aligned);
              next->prev = block;
              next->next = block->next;
              next->size = block->size - size_aligned - header_size;
              block->next = next;
              block->size = size_aligned;
            }
          
          block->size |= 1;
          return __block_addr (block);
        }
      
      block = block->next;
    }
  
  if (NULL == heap_end)
    {
      heap_end = sbrk (size_aligned + header_size);
      if ((void *)-1 == heap_end)
        {
          heap_end = NULL;
          return NULL;
        }
      
      heap_start = heap_end;
      heap_start->size = size_aligned | 1;
      heap_start->next = NULL;
      heap_start->prev = NULL;
      return __block_addr (heap_start);
    }
  
  heap_end->next = sbrk (size_aligned + header_size);
  if ((void *)-1 == heap_end->next)
    {
      heap_end->next = NULL;
      return NULL;
    }
  
  heap_end->next->prev = heap_end;
  heap_end = heap_end->next;
  
  heap_end->next = NULL;
  heap_end->size = size_aligned | 1;
  return __block_addr (heap_end);
}

void
free (void *addr)
{
  if (NULL == addr)
    return;
  
  block_header_t *block = __block_header (addr);
  block->size &= ~(1);
  
  /* Check if this can be attached to the end of a free block */
  if (!(block->prev->size & 1))
    {
      block->prev->size += block->size + header_size;
      block->prev->next = block->next;
      block = block->prev;
    }
  
  /* Check if the next block can be added to the end of this one */
  else if (!(block->next->size & 1))
    {
      block->size += block->next->size;
      block->next = block->next->next;
    }
  
  /* Check if we can shrink the heap */
  if (NULL == block->next)
    {
      block_header_t *prev = block->prev;
      if ((void *)-1 != sbrk (-(block->size + header_size)))
        prev->next = NULL;
    }
}
