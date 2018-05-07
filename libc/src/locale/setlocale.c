#include <locale.h>
#include <string.h>

extern struct lconv locale_c;
extern struct lconv *current_locale;

char *
setlocale (int category, const char *locale)
{
  if (0 == strcmp ("C", locale) || 0 == strcmp ("POSIX", locale))
    return "";
  else
    return NULL;
}
