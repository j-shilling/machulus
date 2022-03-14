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
