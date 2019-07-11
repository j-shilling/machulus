cmake_minimum_required(VERSION 3.13.4)
project(machulus VERSION 0.0.1 LANGUAGES C ASM-ATT)

include_directories(include)

add_subdirectory(src/kernel)
add_subdirectory(src/libc)

add_custom_command(
  OUTPUT iso/boot/machulus
  DEPENDS machulus
  COMMAND cp src/kernel/machulus iso/boot/machulus
  COMMENT "Installing machulus"
  VERBATIM)

add_custom_command(
  OUTPUT machulus.iso
  DEPENDS iso/boot/machulus iso/boot/grub/grub.cfg
  COMMAND grub-mkrescue -o machulus.iso iso
  COMMENT "Generating iso"
  VERBATIM)

add_custom_target(
  iso
  DEPENDS machulus.iso
  VERBATIM)

add_custom_target(
  qemu
  DEPENDS machulus.iso
  COMMAND qemu-system-x86_64 -cpu host -enable-kvm -cdrom machulus.iso
  VERBATIM)
