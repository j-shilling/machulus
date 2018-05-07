#include <locale.h>

extern struct lconv locale_c;
extern struct lconv *current_locale;

struct lconv *
localeconv (void)
{
  if (current_locale)
    return current_locale;
  else
    return &locale_c;
}
