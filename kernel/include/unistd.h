#pragma once

#include <sys/cdefs.h>
#include <stdint.h>

__BEGIN_DECLS

int brk(void*);
void* sbrk(intptr_t);

__END_DECLS
