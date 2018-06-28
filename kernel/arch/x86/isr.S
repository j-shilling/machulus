
.extern interrupt_handler

.macro ISR num
    .global isr_\num
    .type isr_\num, @function
    isr_\num:
        push    $0
        push    $\num
        jmp     isr_common
    .size isr_\num, . - isr_\num
.endm

.macro ISR_WITH_ERR num
    .global isr_\num
    .type isr_\num, @function
    isr_\num:
        push    $\num
        jmp     isr_common
    .size isr_\num, . - isr_\num
.endm

.section .text
    ISR 0
    ISR 1
    ISR 2
    ISR 3
    ISR 4
    ISR 5
    ISR 6
    ISR 7
    ISR_WITH_ERR 8
    ISR 9
    ISR_WITH_ERR 10
    ISR_WITH_ERR 11
    ISR_WITH_ERR 12
    ISR_WITH_ERR 13
    ISR_WITH_ERR 14
    ISR 15
    ISR 16
    ISR_WITH_ERR 17
    ISR 18
    ISR 19
    ISR 20
    ISR 21
    ISR 22
    ISR 23
    ISR 24
    ISR 25
    ISR 26
    ISR 27
    ISR 28
    ISR 29
    ISR_WITH_ERR 30
    ISR 31

    isr_common:
        /* This should only be reachable from a function defined by the ISR
           or ISR_WITH_ERR macros. On the stack is an error code and interrupt
           number */

        pushal      /* Pushes all registers on top of our function parameters */
        
        cld
        push        36(%esp)
        push        36(%esp)
        call        interrupt_handler
        addl        $8, %esp

        popal       /* Get all registers back */
        addl        $8, %esp
        iret
