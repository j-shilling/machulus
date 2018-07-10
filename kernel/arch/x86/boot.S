#define ASM_FILE    1
#include "multiboot.h"
#include "paging.h"

#ifdef HAVE_ASM_USCORE
#  define EXT_C(sym)    _ ## sym
#else
#  define EXT_C(sym)    sym
#endif

#define MULTIBOOT_HEADER_FLAGS  (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE)
#define KPAGE_DIR_FLAGS    (PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE)
#define KPAGE_DIR_INDEX    (KERNEL_VBASE >> 22)
#define FRAME_STACK_DIR_INDEX  (FRAME_STACK_VBASE >> 22)
#define KHEAP_DIR_INDEX    (KHEAP_VBASE >> 22)
#define CR0_PAGING    (1 << 31)
#define CR0_WRITE_PROTECT  (1 << 16)
#define MAX_INDEX               (1024)

.extern kmain
.extern arch_init
.extern kernel_page_directory
.extern kernel_page_table
.extern frames_page_table
.extern kheap_page_table

.section .multiboot
  .align 4
  .long    MULTIBOOT_HEADER_MAGIC
  .long    MULTIBOOT_HEADER_FLAGS
  .long    -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

.section .bss, "aw", @nobits

  .type stack, @object
  .align PAGE_SIZE
  stack:
    stack_bottom:
      .skip PAGE_SIZE
    stack_top:
  .size stack, . - stack

.section .text
  .global _start
  .type _start, @function

     _start:
                /* Put page table in edi */
    movl $(PHYS_ADDR(kernel_page_table)), %edi
                /* Track frame address in esi */
    movl $0, %esi
                /* Set max index in ecx */
    movl $MAX_INDEX, %ecx

  1:  /* Check if we are done */
    cmpl $(PHYS_ADDR(_kernel_end)), %esi
    jge  2f

                movl %esi, %edx
                /* Add entry to table */
    orl  $(KPAGE_FLAGS), %edx
    movl %edx, (%edi)

                /* Interate */
    addl $PAGE_SIZE, %esi
    addl $4, %edi
    loop 1b

  2:  /* Install page directory */
    movl $(PHYS_ADDR(kernel_page_table) + KPAGE_DIR_FLAGS), PHYS_ADDR(kernel_page_directory)
    movl $(PHYS_ADDR(kernel_page_table) + KPAGE_DIR_FLAGS), PHYS_ADDR(kernel_page_directory) + (KPAGE_DIR_INDEX * 4)
    movl $(PHYS_ADDR(kernel_page_directory) + KPAGE_DIR_FLAGS), PHYS_ADDR(kernel_page_directory) + (1023 * 4)

    movl $(PHYS_ADDR(kernel_page_directory)), %ecx
    movl %ecx, %cr3

                /* activate_paging */
    movl %cr0, %ecx
    orl $(CR0_PAGING | CR0_WRITE_PROTECT), %ecx
    movl %ecx, %cr0

    lea higher_half, %ecx
    jmp *%ecx

  higher_half:
    /* Set up the stack */
    mov $stack_top, %esp
    mov $stack_bottom, %ebp

    push %eax /* multiboot magic */
    push %ebx /* multiboot */
    call EXT_C (arch_init)
    addl $8, %esp

    call EXT_C (kmain)
    
    /* kmain should never return and this should be unreachable */
    cli

  inf_loop:
    hlt
    jmp inf_loop
.size _start, . - _start
