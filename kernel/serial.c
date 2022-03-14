#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#include <cpu.h>
#include <serial.h>

#define __port(stream) ((serial_port_t *)stream)->port

struct FILE {
  /**
   * @brief References an implementation of fwrite.
   *
   * @see fwrite
   */
  size_t (*fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);

  /**
   * @brief A spinlock to provide synchronized access to this stream.
   */
  volatile bool lock;
};

static inline void __lock_file(FILE *file) {
  while(!__atomic_test_and_set(&file->lock, __ATOMIC_ACQUIRE));
}

static inline void __unlock_file(FILE *file) {
  __atomic_clear(&file->lock, __ATOMIC_RELEASE);
}

typedef struct {
  FILE __parent;
  uint16_t port;
} serial_port_t;

static size_t
__com1_fwrite(const void *, size_t, size_t, FILE *);

serial_port_t __COM1 = { .__parent = {.fwrite = __com1_fwrite}, .port = 0x3f8};
FILE *COM1 = (FILE *)(&__COM1);

void serial_init(FILE *stream) {
  uint16_t port = __port(stream);

  outb(port + 1, 0x00); // disable interrupts

  outb(port + 3, 0x80); // enable DLAB
  outb(port + 0, 0x03); // set divisor to 3
  outb(port + 1, 0x00);

  outb(port + 3, 0x03); // 8 bits, no parity, one stop bit
  outb(port + 2, 0xc7); // enable fifo, clear with 14-byte threshold
  outb(port + 4, 0x0b); // enable interrupts, rts/dsr set
}

static int serial_empty(uint16_t port) {
  return inb(port + 5) & 0x20;
}

static int serial_putchar(FILE *stream, int c) {
  uint16_t port = __port(stream);
  while (!serial_empty(port));
  outb(port, (char)c);
  return 0;
}

static size_t
__com1_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
  size_t ret = 0;

  const char *cur = (const char *)ptr;
  const char *end = (cur + nmemb);

  for (; cur < end; cur++) {
    const char c = *cur;
    if (serial_putchar(stream, c) < 0) {
      break;
    }
    ret++;
  }

  return ret;
}
