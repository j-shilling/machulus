#pragma GCC diagnostic ignored "-Wundef"

#include <sys/stat.h>

int close(int file) {
    return -1;
}

int lseek(int file, int ptr, int dir) {
    return 0;
}

int read (int file, char *ptr, int len) {
    return 0;
}

#include <stddef.h>
extern int __putchar(int);
int write (int file, char *ptr, int len) {
  size_t ret = 0;

  const char *cur = (const char *)ptr;
  const char *end = (cur + len);

  for (; cur < end; cur++) {
    const char c = *cur;
    if (__putchar((int)c) < 0) {
      break;
    }
    ret++;
  }

  return ret;
}

int isatty(int file) {
  return 1;
}

int fstat(int file, struct stat *st) {
  st->st_mode = S_IFCHR;
  return 0;
}

static char heap[0x1000] = { 0, };
static char *heap_end = heap;

void* sbrk(int incr) {
  char *prev_heap_end;

  prev_heap_end = heap_end;
  if (heap_end + incr > (heap + 0x1000)) {
    write (1, "Heap and stack collision\n", 25);
  }

  heap_end += incr;
  return prev_heap_end;
}
