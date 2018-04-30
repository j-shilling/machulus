#ifndef PRINTF_FSPEC_H
#define PRINTF_FSPEC_H

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

__BEGIN_DECLS

/* The various types a printf argument can be */
struct
__attribute__ ((packed)) __fs_arg
{
  char tag; /* The type field of the % specification */

  union __attribute__ ((packed))
  {
    int int_val;
    long long_val;
    long long long_long_val;
    intmax_t intmax_val;
    size_t size_val;
    ptrdiff_t ptrdiff_val;
    double double_val;
    long double long_double_val;
    void * void_p_val;
  };
};

/* Flag Field */
struct
__attribute__ ((packed)) __fs_flag
{
  unsigned int left : 1; /* - */
  unsigned int plus : 1; /* + */
  unsigned int space : 1; /* ' ' */
  unsigned int zero : 1; /* 0 */
  unsigned int alt : 1; /* # */

  unsigned int padding : 3;
};

/* Length Field*/
struct
__attribute__ ((packed)) __fs_length
{
  unsigned int is_char : 1;
  unsigned int is_short : 1;
  unsigned int is_long : 1;
  unsigned int is_long_long : 1;
  unsigned int is_long_double : 1;
  unsigned int is_size_t : 1;
  unsigned int is_intmax_t : 1;
  unsigned int is_ptrdiff_t : 1;
};

struct
__attribute__ ((packed)) __fspec
{

  struct __fs_flag      flag;
  struct __fs_length    length;
  struct __fs_arg       arg;

  /* Min chars */
  int width;

  /* Max chars */
  int prec;
  
};

int printf_parse_fspec (const char *__fmt,
                        size_t *i,
                        struct __fspec *fspec,
                        va_list ap);

int printf_expand_fspec (char *buf,
                         size_t buf_size,
                         struct __fspec fspec);

__END_DECLS

#endif /* PRINTF_FSPEC_H */

