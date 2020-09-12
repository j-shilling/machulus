#ifndef GDT_H
#define GDT_H

#include <stdint.h>

void gdt_init(void);
void gdt_install(void *, uint16_t, uint16_t);

#endif /* GDT_H */
