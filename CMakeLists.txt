cmake_minimum_required(VERSION 3.10)

set(project "machulus")
set(kernel "machulus")
set(iso-file "${project}.iso")
set(iso-dir "${CMAKE_BINARY_DIR}/iso")

project(${project}
  VERSION 0.1
  DESCRIPTION "An x86_64 microkernel"
  HOMEPAGE_URL https://gitlab.com/shilling.jake/machulus
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

add_custom_command(OUTPUT ${iso-file}
  COMMAND grub-mkrescue -o ${iso-file} ${iso-dir}
  DEPENDS ${kernel})

add_custom_target(iso
  DEPENDS ${iso-file})
