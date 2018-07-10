#define ASM_FILE 1
#include <paging.h>

#define CR0_PAGING          (1 << 31)
#define CR0_WRITE_PROTECT   (1 << 16)

.extern _kernel_end

.section .bss, "aw"

  .type kernel_page_directory, @object
  .align PAGE_SIZE
  kernel_page_directory:
    .skip 4096, 0
  .size kernel_page_directory, . - kernel_page_directory
  
  .type kernel_page_table, @object
  .align PAGE_SIZE
  kernel_page_table:
    .skip 4096, 0
  .size kernel_page_table, . - kernel_page_table
  
.section .text

  .type paging_init, @function
  .global paging_init
  paging_init:
  
    movl $(PHYS_ADDR(kernel_page_table)), %edi
    movl $0, %esi
    movl $1024, %ecx

  1: /* Map from 0 to _kernel_end */
    cmpl $(PHYS_ADDR(_kernel_end)), %esi
    jge  2f

    movl %esi, %edx
    orl  $(KPAGE_FLAGS), %edx
    movl %edx, (%edi)

    addl $PAGE_SIZE, %esi
    addl $4, %edi
    loop 1b

  2: /* Install page directory */
    movl $(PHYS_ADDR(kernel_page_table) + 3), PHYS_ADDR(kernel_page_directory)
    movl $(PHYS_ADDR(kernel_page_table) + 3), PHYS_ADDR(kernel_page_directory) + ((KERNEL_VBASE >> 22) * 4)
    movl $(PHYS_ADDR(kernel_page_directory) + 3), PHYS_ADDR(kernel_page_directory) + (1023 * 4)

    movl $(PHYS_ADDR(kernel_page_directory)), %ecx
    movl %ecx, %cr3

     /* activate_paging */
    movl %cr0, %ecx
    orl $(CR0_PAGING | CR0_WRITE_PROTECT), %ecx
    movl %ecx, %cr0
    ret
    
  .size paging_init, . - paging_init