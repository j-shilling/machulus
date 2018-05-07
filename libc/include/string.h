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

/**
 * Appends the src string to the end of dest overriding the terminating NULL
 * character. Buffers must not overlap and dest must be big enough for the 
 * result. Returns a pointer to dest.
 */
char *strcat(char *dest, const char *src);

/**
 * Appends the first n bytes of src to the end of dest overriding the terminating
 * NULL character. Buffers must not overlap and dest must be big enough for the 
 * result. Returns a pointer to dest.
 */
char *strncat (char *dest, const char *src, size_t n);

/**
 * Returns a pointer to the first occurance of c in s; NULL if not found.
 */
char *strchr (const char *s, int c);

/**
 * Returns a pointer to the last occurance of c in s; NULL if not found.
 */
char *strrchr (const char *, int);

/**
 * Function compares the two strings s1 and s2.  It returns
 * an integer less than, equal to, or greater than zero if  s1  is  found,
 * respectively, to be less than, to match, or be greater than s2.
 */
int strcmp (const char *s1, const char *s2);

/**
 * Similar to strcmp(), except it compares only the first (at most) n bytes 
 * of s1 and s2.
 */
int strncmp (const char *s1, const char *s2, size_t n);

int strcoll (const char *, const char *);

/**
 * The  strcpy()  function  copies the string pointed to by src, including the 
 * terminating null byte ('\0'), to the buffer  pointed  to  by  dest.The  
 * strings  may  not overlap, and the destination string dest must belarge 
 * enough to receive the copy.
 */
char *strcpy (char *dest, const char *src);

/**
 * The  strncpy()  function is similar, except that at most n bytes of src are 
 * copied.  Warning: If there is no null byte among the first n  bytes of src, 
 * the string placed in dest will not be null-terminated. If  the  length of src
 * is less than n, strncpy() writes additional null bytes to dest to ensure that
 * a total of n bytes are written.
 */
char *strncpy (char *dest, const char *src, size_t n);

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
