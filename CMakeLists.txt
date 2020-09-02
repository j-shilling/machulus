cmake_minimum_required(VERSION 3.5)

project(Machulus VERSION 0.1
  DESCRIPTION "A microkernel"
  LANGUAGES C ASM)

add_subdirectory(libc)
add_subdirectory(kernel)

set(GRUB_CFG ${CMAKE_CURRENT_BINARY_DIR}/grub.cfg)
add_custom_command(
  OUTPUT ${GRUB_CFG}
  COMMAND "${CMAKE_C_COMPILER}"
  ARGS -E -P -x c -o ${GRUB_CFG} ${CMAKE_SOURCE_DIR}/res/grub.cfg.in
  DEPENDS ${CMAKE_SOURCE_DIR}/res/grub.cfg.in)
add_custom_target(
  grub_cfg
  DEPENDS ${GRUB_CFG})

set(ISODIR ${CMAKE_BINARY_DIR}/iso)
add_custom_command(
  OUTPUT machulus.iso
  COMMAND ${CMAKE_COMMAND}
  ARGS -E make_directory ${ISODIR}/boot/grub
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy_if_different ${GRUB_CFG} ${ISODIR}/boot/grub
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy_if_different $<TARGET_FILE:machulus> ${ISODIR}/boot/
  COMMAND grub-mkrescue
  ARGS -o machulus.iso ${ISODIR}
  DEPENDS grub_cfg machulus)
add_custom_target(
  iso
  DEPENDS machulus.iso)
add_custom_target(
  qemu
  DEPENDS iso
  COMMAND qemu-system-x86_64 -cpu host -enable-kvm -cdrom machulus.iso)
