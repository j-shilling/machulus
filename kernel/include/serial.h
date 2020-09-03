#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stdio.h>

extern FILE *COM1;
void serial_init(FILE *);

#endif /* SERIAL_H */
