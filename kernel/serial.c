#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <cpu.h>
#include <serial.h>

static const uint16_t COM1 = 0x3f8;

static int serial_empty(uint16_t port) {
  return inb(port + 5) & 0x20;
}

static int serial_putchar(uint16_t port, int c) {
  while (!serial_empty(port));
  outb(port, (char)c);
  return 0;
}

ssize_t serial_write(const void *buf, size_t count) {
  ssize_t ret = 0;

  const char *cur = (const char *)buf;
  const char *end = (cur + count);

  for (; cur < end; cur++) {
    const char c = *cur;
    if (serial_putchar(COM1, c) < 0) {
      break;
    }
    ret++;
  }

  return ret;
}

void serial_init(void) {
  outb(COM1 + 1, 0x00); // disable interrupts

  outb(COM1 + 3, 0x80); // enable DLAB
  outb(COM1 + 0, 0x03); // set divisor to 3
  outb(COM1 + 1, 0x00);

  outb(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
  outb(COM1 + 2, 0xc7); // enable fifo, clear with 14-byte threshold
  outb(COM1 + 4, 0x0b); // enable interrupts, rts/dsr set
}
