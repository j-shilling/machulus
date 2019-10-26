cmake_minimum_required(VERSION 3.10)

set(CMAKE_SYSTEM_PROCESSOR x86_64)

project(machulus
  VERSION 0.1
  DESCRIPTION "An x86_64 microkernel"
  HOMEPAGE_URL https://gitlab.com/shilling.jake/machulus
  LANGUAGES C ASM-ATT)

set(CMAKE_C_COMPILER x86_64-elf-gcc)
set(CMAKE_ASM-ATT_COMPILER x86_64-elf-gcc)
set(ENV{LDEMULATION} "elf_i386")

add_executable(machulus
  start.s
  kernel.c)

target_compile_options(machulus
  PRIVATE -ffreestanding -m32 -c)

target_link_options(machulus
  PRIVATE -nostdlib -ffreestanding -T ${CMAKE_SOURCE_DIR}/linker.ld)
