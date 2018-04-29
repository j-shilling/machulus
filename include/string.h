#ifndef _STRING_H_
#define _STRING_H_

#include <sys/cdefs.h>
#include <stddef.h>

__BEGIN_DECLS

int memcmp (const void *, const void *, size_t);
void *memcpy (void *restrict, const void * restrict, size_t);
void *memmove (void *, const void *, size_t);
void *memset (void *, int, size_t);
size_t strlen (const char *);

__END_DECLS

#endif /* _STRING_H_ */
