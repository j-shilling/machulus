cmake_minimum_required(VERSION 3.10)

set(project "machulus")
set(kernel "machulus")
set(iso-file "${project}.iso")
set(iso-dir "${CMAKE_SOURCE_DIR}/iso")

project(${project}
  VERSION 0.1
  DESCRIPTION "An x86_64 microkernel"
  LANGUAGES C ASM-ATT)

# Try to find a gcc cross compiler
find_program(xcc
  x86_64-elf-gcc)
if (${xcc} STREQUAL "xcc-NOTFOUND")
  message(FATAL_ERROR
    "This project must be build with a GCC cross compiler, "
    "but x86_64-elf-gcc could not be found.")
endif()

set(CMAKE_C_COMPILER ${xcc})
set(CMAKE_ASM-ATT_COMPILER ${xcc})

add_executable(${kernel}
  start.s
  multiboot_memory_map.s
  multiboot_available_ram.s
  multiboot_memory_map_entry_next_page.s
  kernel.c)

target_compile_options(${kernel}
  PRIVATE -ffreestanding -mcmodel=kernel -mno-red-zone -ggdb3 -c)

target_link_options(${kernel}
  PRIVATE -nostdlib -ffreestanding -n -T ${CMAKE_SOURCE_DIR}/linker.ld)

set_target_properties(${kernel}
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${iso-dir}/boot")

# Check if grub-mkrescue is on the system and, if it is, create a make
# target to generate an .ISO file
find_program(grub-mkrescue
  grub-mkrescue)
if (${grub-mkrescue} STREQUAL "grub-mkrescue-NOTFOUND")
  message(WARNING
    "Could not find grub-mkrescue, so you won't be able to "
    "run make iso, make qemu, or make debug.")
else()
  # Create grub.cfg for iso image
  configure_file(${CMAKE_SOURCE_DIR}/grub.cfg.in ${iso-dir}/boot/grub/grub.cfg
    NEWLINE_STYLE UNIX)

  add_custom_command(OUTPUT ${iso-file}
    COMMAND ${grub-mkrescue} -o ${iso-file} ${iso-dir}
    DEPENDS ${kernel} ${iso-dir}/boot/grub/grub.cfg)

  add_custom_target(iso
    DEPENDS ${iso-file})

  # Check if qemu is on the system and, if it is, create a make target
  # to run the generated .ISO image
  find_program(qemu
    qemu-system-x86_64)
  if (${qemu} STREQUAL "qemu-NOTFOUND")
    message(WARNING
      "Could not find qemu-system-x86_64, so you won't be "
      "able to run make qemu or make debug.")
  else()
    add_custom_target(qemu
      COMMAND ${qemu} -cpu host -enable-kvm -cdrom ${iso-file}
      DEPENDS ${iso-file})
    add_custom_target(debug
      COMMAND ${qemu} -cpu host -enable-kvm -cdrom ${iso-file} -monitor stdio -d int -s -S
      DEPENDS ${iso-file})
  endif()
endif()
