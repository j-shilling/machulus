#include <stddef.h>
#include <unistd.h>

#include <machulus/tty.h>

#ifndef _READ_WRITE_RETURN_TYPE
#define _READ_WRITE_RETURN_TYPE ssize_t
#endif

_READ_WRITE_RETURN_TYPE write(int file, const void *ptr, size_t len) {
  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
    return tty_write(ptr, len);
  }

  return 0;
}
