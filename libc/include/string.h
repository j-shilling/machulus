#ifndef __STRING_H__
#define __STRING_H__

#include <sys/cdefs.h>
#include <stddef.h>

__BEGIN_DECLS

/**
 * Copy n bytes of memory from src to dest. Areas may not
 * overlap.
 */
void *memcpy (void *dest, const void *src, size_t n);

/**
 * Copy n bytes of memory from src to dest. Areas may 
 * overlap.
 */
void *memmove (void *dest, const void *src, size_t n);

/**
 * Scans the first n bytes of s and returns a pointer indicating
 * the first byte equal to c. Returns NULL if c is not found. Both
 * s and c are read as an unsigned char
 */
void *memchr (const void *s, int c, size_t n);

/**
 * Compares the first n bytes of s1 and s2. Each byte is read as
 * an unsigned char. Returns an int less than, equal to, or greater
 * than zero, if s1 is found to be less than, equal to, or greater
 * than s2. If n is zero, then zero is returned.
 */
int memcmp (const void *s1, const void *s2, size_t n);

/**
 * Sets the first n bytes of s to the constant byte value c. Returns
 * a pointer to s
 */
void *memset (void *s, int c, size_t n);
char *strcat(char *,const char *);
char *strncat (char *, const char *, size_t);
char *strchr (const char *, int);
char *strrchr (const char *, int);
int strcmp (const char *, const char *);
int strncmp (const char *, const char *, size_t);
int strcoll (const char *, const char *);
char *strcpy (char *, const char *);
char *strncpy (char *, const char *, size_t);
char *strerror (int);
size_t strlen (const char *);
size_t strspn (const char *, const char *);
size_t strcspn (const char *, const char *);
char *strpbrk (const char*, const char *);
char *strstr (const char *, const char *);
char *strtok (char *, const char *);
size_t strxfrm (char *, const char *, size_t);

__END_DECLS

#endif /* __STRING_H__ */
