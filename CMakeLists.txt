cmake_minimum_required(VERSION 3.5)

project(Machulus VERSION 0.1
  DESCRIPTION "A microkernel"
  LANGUAGES C ASM)

add_executable(machulus
  src/boot/boot_gdt.S
  src/boot/boot_page_tables.S
  src/boot/boot_puts.S
  src/boot/multiboot_header.S
  src/boot/start32.S
  src/boot/start64.S
  src/kernel.c
  src/stdlib/stdio/fputc.c
  src/stdlib/stdio/fputs.c
  src/stdlib/stdio/fwrite.c
  src/stdlib/stdio/puts.c
  src/stdout.c)

target_include_directories(machulus PRIVATE inc inc/contrib)
target_compile_options(machulus PRIVATE
  -target x86_64-pc-none-elf
  -ffreestanding
  -mcmodel=kernel
  -mno-red-zone
  -fno-pic
  -ffunction-sections
  -fdata-sections)

target_link_options(machulus PRIVATE
  -nostdlib
  -no-pie
  -Wl,--build-id=none
  -Wl,-T${CMAKE_BINARY_DIR}/res/linker.ld
  -Wl,--gc-sections)
add_dependencies(
  machulus
  linker_ld)

set(INFILES
  ${CMAKE_SOURCE_DIR}/res/linker.ld.in
  ${CMAKE_SOURCE_DIR}/res/grub.cfg.in)
foreach(_filename ${INFILES})
  get_filename_component(_file ${_filename} NAME)
  string(REPLACE ".in" "" _resource ${_file})
  string(REPLACE "." "_" _target ${_resource})

  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/res/${_resource}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/res
    COMMAND "${CMAKE_C_COMPILER}" -I${CMAKE_SOURCE_DIR}/inc -E -P -x c -o ${CMAKE_BINARY_DIR}/res/${_resource} ${_filename}
    COMMENT "Preprocessing ${_file}"
    DEPENDS ${_filename}
    )

  add_custom_target(
    ${_target}
    DEPENDS ${CMAKE_BINARY_DIR}/res/${_resource}
    )
endforeach()

set(ISODIR ${CMAKE_BINARY_DIR}/iso)
add_custom_command(
  OUTPUT ${ISODIR}/boot/grub/grub.cfg
  COMMAND ${CMAKE_COMMAND} -E make_directory ${ISODIR}/boot/grub
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/res/grub.cfg ${ISODIR}/boot/grub
  DEPENDS ${CMAKE_BINARY_DIR}/res/grub.cfg
  )
add_custom_command(
  OUTPUT ${ISODIR}/boot/machulus
  COMMAND ${CMAKE_COMMAND} -E make_directory ${ISODIR}/boot
  COMMAND ${CMAKE_COMMAND} -E copy_if_different machulus ${ISODIR}/boot/
  DEPENDS machulus
  )
add_custom_command(
  OUTPUT machulus.iso
  COMMAND grub-mkrescue -o machulus.iso ${ISODIR}
  DEPENDS ${ISODIR}/boot/machulus ${ISODIR}/boot/grub/grub.cfg
  )
add_custom_target(
  iso
  DEPENDS machulus.iso
  )

add_custom_target(
  qemu
  DEPENDS iso
  COMMAND qemu-system-x86_64 -cpu host -enable-kvm -cdrom machulus.iso
  )
