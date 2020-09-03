#ifndef CPU_H
#define CPU_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t value) {
  __asm__ volatile ("outb %0, %1" :: "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

#endif /* CPU_H */
