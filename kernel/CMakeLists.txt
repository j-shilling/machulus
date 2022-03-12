set(LINKER_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/linker.ld)
set(KERNEL machulus)

add_executable(${KERNEL}
  boot/boot_gdt.S
  boot/boot_page_tables.S
  boot/boot_puts.S
  boot/multiboot_header.c
  boot/start32.S
  boot/start64.S
  kernel.c
  stdout.c)
target_include_directories(${KERNEL} PRIVATE
  include
  include/contrib)
target_compile_options(${KERNEL} PRIVATE
  -ffreestanding
  -mcmodel=kernel
  -mno-red-zone
  -fno-pic
  -ffunction-sections
  -fdata-sections)
target_link_options(${KERNEL} PRIVATE
  -nostdlib
  -no-pie
  -n
  -Wl,--build-id=none
  -Wl,-T${LINKER_SCRIPT}
  -Wl,--gc-sections)
target_link_libraries(machulus libc)
target_link_libraries(machulus sys)

add_custom_command(
  TARGET ${KERNEL} PRE_LINK
  COMMAND "${CMAKE_C_COMPILER}"
  ARGS -E -P -x c -o ${LINKER_SCRIPT} ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld.in
  COMMENT "Preprocessing linker.ld"
  DEPENDS linker.ld.in)
