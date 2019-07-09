set(ARCH_SOURCES x86_64/boot.S
  x86_64/early-idt.S
  x86_64/framebuffer.c
  x86_64/halt.c
  x86_64/kernel-exception-handler.c
  x86_64/stdout.c)

set(KERNEL_SOURCES kernel.c
  panic.c)

include_directories(x86_64)

add_library(kernel STATIC
  ${ARCH_SOURCES}
  ${KERNEL_SOURCES})
