#define ASM_FILE 1
#include <paging.h>
#include <config.h>

#define CR0_PAGING          (1 << 31)
#define CR0_WRITE_PROTECT   (1 << 16)

.extern _kernel_end

#if __x86_64__

#define PAGE_SHIFT          12
#define PAGE_SIZE           (1 << PAGE_SHIFT)
#define PAGE_MASK           (PAGE_SIZE - 1)

#define PTE_SHIFT           (PAGE_SHIFT + (9*0))
#define PDE_SHIFT           (PAGE_SHIFT + (9*1))
#define PDPE_SHIFT          (PAGE_SHIFT + (9*2))
#define PML4E_SHIFT         (PAGE_SHIFT + (9*3))

#define PTE(x)              (((x) >> PTE_SHIFT) & 0x1FF)
#define PDE(x)              (((x) >> PDE_SHIFT) & 0x1FF)
#define PDPE(x)             (((x) >> PDPE_SHIFT) & 0x1FF)
#define PML4E(x)            (((x) >> PML4E_SHIFT) & 0x1FF)

#define PF_P                (1 << 0) /* Present */
#define PF_RW               (1 << 1) /* Read/Write */
#define PF_USER             (1 << 2) /* User */
#define PF_WRITE_THROUGH    (1 << 3)
#define PF_DISABLE_CACHE    (1 << 4)

.extern kernel_size
.extern kernel_start

.section .text

  .type paging_init, @function
  .global paging_init
  paging_init:
  
    /* We need to calculate the number of pages taken up by the
       page structures themselfs. Get the size of the kernel in eax
       and the end of the kernel in ebx */
    mov $kernel_size, %eax
    add $PAGE_MASK, %eax
    and $(~PAGE_MASK), %eax
    
    mov %eax, %ebx
    add $kernel_start, %ebx
    
    /* Set up for our counting loop. ecx stores the index in a current
       page table and edx counts the number of pages needed. Initialize
       eax to for because we have at least 1 of each page structure */
    mov $PTE(kernel_start), %ecx
    mov $4, %edx
    
1:  sub $PAGE_SIZE, %eax
    cmp $512, %ecx
    jne 2f
    
    inc %edx
    mov $0, %ecx
    jmp 3f
    
2:  inc %ecx
3:  cmp $0, %eax
    jne 1b
    
    /* Now ebx holds the page aligned end of the kernel and edx the number
       of pages needed to map the kernel. We will use the next several pages
       to store the structures and we need to zero these addresses */
    mov %edx, %ecx
    shl $PAGE_SHIFT, %ecx
    add %ebx, %ecx
    add $PAGE_SIZE, %ecx
    
    mov %ebx, %eax
4:  mov $0, (%eax)
    inc %eax
    cmp %ecx, %eax
    jne 4b
    
    /* We want to create two mappings: one identify map of KERNEL_START and
       one for the higher half kernel --
       
       KERNEL_START = 0x100000
       PML4E = 0
       PDPE  = 0
       PDE   = 0
       PTE   = 256
       
       higher half = 
       PML4E = 511
       PDPE  = 510
       PDE   = 0
       PTE   = 256 */
    
    /* First we init the PML4. Map it to itself at index 510 */
    mov %ebx, %eax
    add $(8 * 510), %eax
    mov %ebx, %edx
    or  $(PF_RW | PF_P), %edx
    mov %edx, (%eax)
    
    mov %ebx, %eax
    add $(8 * PML4E(KERNEL_START)), %eax
    mov %ebx, %edx
    add $PAGE_SIZE, %edx
    or  $(PF_RW | PF_P), %edx
    mov %edx, (%eax)
    
    mov %ebx, %eax
    add $(8 * PML4E(KERNEL_VBASE | KERNEL_START)), $eax
    mov %edx, (%eax)
    
    /* Now the PDP */
    mov %ebx, %eax
    add $((8 * PDPE(KERNEL_START)) + PAGE_SIZE), %eax
    mov %ebx, %edx
    add $(2 * PAGE_SIZE), %edx
    or  $(PF_RW | PF_P), %edx
    mov %edx, (%eax)
    
    mov %ebx, %eax
    add $((8 * PDPE(KERNEL_VBASE | KERNEL_START)) + PAGE_SIZE), %eax
    mov %ebx, %edx
    add $(2 * PAGE_SIZE), %edx
    or  $(PF_RW | PF_P), %edx
    mov %edx, (%eax)
    ret
    
  .size paging_init, . - paging_init

#else

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
  
#endif