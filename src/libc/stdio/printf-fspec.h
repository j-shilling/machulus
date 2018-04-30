#ifndef PRINTF_FSPEC_H
#define PRINTF_FSPEC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  struct fspec
  {
    /* Flag Field */
    struct
    {
      unsigned int left:1; /* - */
      unsigned int plus:1; /* + */
      unsigned int space:1; /* ' ' */
      unsigned int zero:1; /* 0 */
      unsigned int alt:1; /* # */
      
      unsigned int padding:3;
    };
    
    /* Length Field*/
    struct
    {
      unsigned int is_char:1;
      unsigned int is_short:1;
      unsigned int is_long:1;
      unsigned int is_long_long:1;
      unsigned int is_long_double:1;
      unsigned int is_size_t:1;
      unsigned int is_intmax_t:1;
      unsigned int is_ptrdiff_t:1;
    };
    
    /* Type Field */
    char type;
    
    /* Min chars */
    int width;
    
    /* Max chars */
    int prec;
  };
  
  /* __fmt should point to the leading '%' character. This function will
   parse the format specify and save the result in the fspec parameters. 0 is
   returned on success. */
  int printf_parse_fspec (const char *__fmt, size_t *i, struct fspec *fspec);

#ifdef __cplusplus
}
#endif

#endif /* PRINTF_FSPEC_H */

