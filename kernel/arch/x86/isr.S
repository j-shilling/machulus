
.extern interrupt_handler

.macro ISR num
    .global isr_\num
    .type isr_\num, @function
    isr_\num:
        pusha
        cld
        push $0
        push $\num
        call interrupt_handler
        addl $8, %esp
        popa
        iret
    .size isr_\num, . - isr_\num
.endm

.macro ISR_WITH_ERR num
    .global isr_\num
    .type isr_\num, @function
    isr_\num:
        pusha
        cld
        push $\num
        call interrupt_handler
        addl $8, %esp
        popa
        iret
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
    ISR 8
    ISR 9
    ISR 10
    ISR 11
    ISR 12
    ISR 13
    ISR 14
    ISR 15
    ISR 16
    ISR 17
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
    ISR 30
    ISR 31
