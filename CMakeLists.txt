cmake_minimum_required(VERSION 3.12)

project(
  Machulus VERSION 0.1
  DESCRIPTION "A microkernel"
  LANGUAGES C ASM
)

set(
  DEBUG_COMPILE_OPTIONS
  -ggdb
  -O0
  -Werror
  -Wall
  -Wextra
  -Wshadow
  -Wdouble-promotion
  -Wformat=2
  -Wundef
  -fno-common
  -Wno-unused-parameter
)
set(
  FREESTANDING_COMPILE_OPTIONS
  -ffreestanding
  -mcmodel=kernel
  -mno-red-zone
  -fno-pic
)

set_directory_properties(PROPERTIES
  INCLUDE_DIRECTORIES ${CMAKE_SORUCE_DIR}/include)

add_subdirectory(newlib)
add_subdirectory(kernel)

set(GRUB_CFG ${CMAKE_CURRENT_BINARY_DIR}/grub.cfg)
add_custom_command(
  OUTPUT ${GRUB_CFG}
  COMMAND "${CMAKE_C_COMPILER}"
  ARGS -E -P -x c -o ${GRUB_CFG} ${CMAKE_SOURCE_DIR}/res/grub.cfg.in
  DEPENDS ${CMAKE_SOURCE_DIR}/res/grub.cfg.in
)
add_custom_target(
  grub_cfg
  DEPENDS ${GRUB_CFG}
)

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
  DEPENDS grub_cfg machulus
)
add_custom_target(
  iso
  DEPENDS machulus.iso
)
add_custom_target(
  qemu
  DEPENDS iso
  COMMAND qemu-system-x86_64 -cpu host -enable-kvm -cdrom machulus.iso -serial stdio
)
