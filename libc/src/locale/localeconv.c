#include <locale.h>
#include <string.h>

extern struct lconv *locale;

struct lconv *
localeconv (void)
{
  return locale;
}
