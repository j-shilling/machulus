#include <stdarg.h>
#include <stdio.h>

#include <ubsan.h>

__attribute__((format (printf, 2, 3)))
void __ubsan_printf(struct ubsan_source_location *loc, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  __ubsan_vprintf(loc, fmt, ap);
  va_end(ap);
}

void __ubsan_vprintf(struct ubsan_source_location *loc, const char *fmt,
                     va_list ap) {
    fprintf(stderr, "UBSAN %s:%d:%d - ", loc->filename, loc->line, loc->column);
    fprintf(stderr, fmt, ap);
}
