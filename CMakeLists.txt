cmake_minimum_required(VERSION 3.10)

set(project "machulus")
set(kernel "machulus")
set(iso-file "${project}.iso")
set(iso-dir "${CMAKE_SOURCE_DIR}/iso")

project(${project}
  VERSION 0.1
  DESCRIPTION "An x86_64 microkernel"
  LANGUAGES C ASM-ATT)

set(CMAKE_C_COMPILER x86_64-elf-gcc)
set(CMAKE_ASM-ATT_COMPILER x86_64-elf-gcc)

add_executable(${kernel}
  start.s
  kernel.c)

target_compile_options(${kernel}
  PRIVATE -ffreestanding -m32 -c)

target_link_options(${kernel}
  PRIVATE -nostdlib -ffreestanding -T ${CMAKE_SOURCE_DIR}/linker.ld)

set_target_properties(${kernel}
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${iso-dir}/boot")

# Check if grub-mkrescue is on the system and, if it is, create a make
# target to generate an .ISO file
find_program(grub-mkrescue
  grub-mkrescue)
if (NOT ${grub-mkrescue} EQUAL "grub-mkrescue-NOTFOUND")
  add_custom_command(OUTPUT ${iso-file}
    COMMAND ${grub-mkrescue} -o ${iso-file} ${iso-dir}
    DEPENDS ${kernel} ${iso-dir}/boot/grub/grub.cfg)

  add_custom_target(iso
    DEPENDS ${iso-file})

  # Check if qemu is on the system and, if it is, create a make target
  # to run the generated .ISO image
  find_program(qemu
    qemu-system-x86_64)
  if (NOT ${qemu} EQUAL "qemu-NOTFOUND")

    message ("${CMAKE_HOST_SYSTEM}")
    add_custom_target(qemu
      COMMAND ${qemu} -cpu host -enable-kvm -cdrom ${iso-file}
      DEPENDS ${iso-file})
  endif()
endif()
