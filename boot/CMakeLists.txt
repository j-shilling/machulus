add_library(boot
  start.s
  multiboot_memory_map.s
  multiboot_available_ram.s
  multiboot_memory_map_entry_next_page.s)

target_compile_options(boot
  PRIVATE -ffreestanding -mcmodel=kernel -mno-red-zone -ggdb3 -c)
