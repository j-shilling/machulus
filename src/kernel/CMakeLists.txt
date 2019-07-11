set(__64bits__ 1)

set(ARCH_ASM_SOURCES
  x86_64/boot.S
  x86_64/early-idt.S)

set(ARCH_C_SOURCES
  x86_64/framebuffer.c
  x86_64/halt.c
  x86_64/kernel-exception-handler.c
  x86_64/stdout.c)

set(KERNEL_SOURCES
  kernel.c
  panic.c)

set_source_files_properties(${ARCH_ASM_SOURCES} PROPERTIES LANGUAGE C)

configure_file(config.h.in config.h)

include_directories(x86_64)

add_executable(machulus
  ${ARCH_ASM_SOURCES}
  ${ARCH_C_SOURCES}
  ${KERNEL_SOURCES})

target_compile_options(machulus PRIVATE -mcmodel=kernel -mno-red-zone -ffreestanding -no-pie -fno-pic -fno-stack-protector -imacros config.h)
target_link_options(machulus PRIVATE -nostdlib -mcmodel=kernel -fno-PIC -no-pie -T ldscript)
target_link_libraries(machulus k)

add_custom_command(
  OUTPUT ldscript
  COMMAND "${CMAKE_C_COMPILER}" -E -x c -P x86_64/ldscript.in -Ix86_64 -imacros config.h -o ldscript
  MAIN_DEPENDENCY x86_64/ldscript.in
  COMMENT "Preprocessing ldscript.in"
  VERBATIM)

add_custom_target(
  linker_script
  DEPENDS ldscript
  COMMENT "Generating linker script"
  VERBATIM)

add_dependencies(machulus linker_script)

add_custom_command(
  TARGET machulus POST_BUILD
  COMMAND cp machulus ../../iso/boot/machulus
  COMMENT "Installing machulus"
  VERBATIM)

