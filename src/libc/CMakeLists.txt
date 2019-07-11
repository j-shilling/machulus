set(LIBC_SOURCES errno/errno.c
  stdio/fprintf.c
  stdio/fputc.c
  stdio/fputs.c
  stdio/printf.c
  stdio/putchar.c
  stdio/puts.c
  stdio/snprintf.c
  stdio/vfprintf.c
  stdio/vprintf.c
  stdio/vsnprintf.c)

add_library(k STATIC
  ${LIBC_SOURCES})

target_compile_options(k PRIVATE -mcmodel=kernel -mno-red-zone -ffreestanding -no-pie -fno-pic -fno-stack-protector)
