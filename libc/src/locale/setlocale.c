#include <locale.h>
#include <string.h>

char *
setlocale (int category, const char *locale)
{
  if (NULL == locale
      || 0 == strcmp ("C", locale)
      || 0 == strcmp ("POSIX", locale))
    return "C";
  else
    return NULL;
}
