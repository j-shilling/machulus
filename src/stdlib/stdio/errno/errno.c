static int errno = 0;

// Eventually, this function will do whatever logic will keep errno
// thread-local. For now, it'll just return a constant memory address.
inline int *__errno_address(void) {
  return &errno;
}
