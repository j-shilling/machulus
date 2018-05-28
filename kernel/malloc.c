#include <stdlib.h>
#include <unistd.h>

#define	  ALIGNMENT   8
#define	  __align(x)  (((x) + ALIGNMENT - 1) & ~(ALIGNMENT - 1))

#define	  __block_size(x) (*(x) & 0xFFFFFFFE)
#define	  __block_is_free(x)  ((*(x)) & 1)
#define	  __block_next(x) ((size_t *)((char *)(x) + __block_size (x) + header_size))
#define	  __block_addr(x) ((void *)((char *)(x) + header_size))

void *
malloc (size_t size)
{
  static size_t header_size = __align (sizeof (size_t));
  static void *heap_start = NULL;
  static void *heap_end;
  if (NULL == heap_start)
    {
      heap_start = sbrk (0);
      heap_end = heap_start;
    }

  if ((void *)-1 == heap_start)
    return NULL; /* could not initialize the heap */

  size_t size_aligned = __align (size);

  /* search heap for a free block */
  size_t *block = heap_start;
  while ((void *)block < heap_end)
    {
      size_t *next = __block_next (block);

      /* last bit of the size_t marks freed */
      if (__block_is_free (block))
	{
	  size_t available = __block_size (block);
	  if (available >= size_aligned)
	    {
	      /* Is this worth breaking up */
	      if ((available - size_aligned - header_size) > ALIGNMENT)
		{
		  (*block) = size_aligned;
		  size_t *new_block = __block_next (block);
		  (*new_block) = available - size_aligned - header_size;
		}

	      return __block_addr (block);
	    }

	  /* check if we can combine blocks */
	  if (((void *)next < heap_end) && ((*next) & 1))
	    (*block) += (__block_size (next) + header_size);
	}
      
      block = next;
    }

  /* we need to grow the heap */
  block= sbrk (size_aligned + header_size);
  if ((void *)-1 == block)
    return NULL;

  heap_end = block;
  (*block) = size_aligned;
  return __block_addr (block);
}

void
free (void *addr)
{
  if (NULL != addr)
    {
      size_t *header = ((size_t *)addr) - 1;
      (*header) &= 0x00000001;
    }
}
