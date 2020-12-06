set(LIBC_SRCS
  stdio/fputc.c
  stdio/fputs.c
  stdio/fwrite.c
  stdio/puts.c)

add_library(k STATIC
  ${LIBC_SRCS})

target_compile_options(k PUBLIC
  -ffreestanding
  -mcmodel=kernel
  -mno-red-zone
  -fno-pic
  -ffunction-sections
  -fdata-sections)

target_include_directories(k PUBLIC include)

target_compile_definitions(k PRIVATE __KERNEL__)
