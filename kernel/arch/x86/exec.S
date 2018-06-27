#define ASM_FILE                        1

#include "gdt.h"

#define USER_DATA_SELECTOR             (GDT_USER_DATA | DPL_USER)
#define USER_CODE_SELECTOR             (GDT_USER_CODE | DPL_USER)

.extern test_func

.section .text
    
    .global exec_usermode
    .type exec_usermode, @function
    /* void exec_usermode (void (*func_ptr)(void)) */
    exec_usermode:

        mov         $USER_DATA_SELECTOR, %ax
        mov         %ax, %ds
        mov         %ax, %es
        mov         %ax, %fs
        mov         %ax, %gs

        mov         %esp, %eax
        pushl       $USER_DATA_SELECTOR
        pushl       %eax
        pushf
        pushl       $USER_CODE_SELECTOR
        pushl       $test_func
        iret

    .size exec_usermode, . - exec_usermode