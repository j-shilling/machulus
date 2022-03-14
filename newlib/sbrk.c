#include <unistd.h>

static char heap[0x1000] = { 0, };
static char *heap_end = heap;

void* sbrk(intptr_t incr) {
  char *prev_heap_end;

  prev_heap_end = heap_end;
  if (heap_end + incr > (heap + 0x1000)) {
    write (1, "Heap and stack collision\n", 25);
  }

  heap_end += incr;
  return prev_heap_end;
}
