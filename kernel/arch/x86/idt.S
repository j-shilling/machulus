.section .data, "", @progbits

    .type idt, @object
    idt:

        .skip (8 * 256), 0x0
    
    idt_end:
    .size idt, . - idt

.section .rodata, "", @progbits
    
    .type idt_descriptor, @object
    idt_descriptor:
   
        .2byte      idt_end - idt - 1
        .4byte      idt

    .size idt_descriptor, . - idt_descriptor

.section .text

    .type idt_init, @function
    .global idt_init
    idt_init:

        lidt        idt_descriptor
        ret

    .size idt_init, . - idt_init

    /* void idt_register_trap (int index, uint32_t handler_addr) */
    .type idt_register_trap, @function
    .global idt_register_trap
    idt_register_trap:

        push        %ebp
        movl        %esp, %ebp

        movl        $8, %edi
        imul        8(%ebp), %edi
        addl        $idt, %edi

        movl        12(%ebp), %edx
        andl        $0x0000FFFF, %edx
        orl         $0x00080000, %edx
        movl        %edx, (%edi)
        addl        $4, %edi

        movl        12(%ebp), %edx
        andl        $0xFFFF0000, %edx
        orl         $0x00008F00, %edx
        movl        %edx, (%edi)

        popl        %ebp
        ret

    .size idt_register_trap, . - idt_register_trap

    /* void idt_register_interrupt (int index, uint32_t handler_addr) */
    .type idt_register_interrupt, @function
    .global idt_register_interrupt
    idt_register_interrupt:

        push        %ebp
        movl        %esp, %ebp

        movl        $8, %edi
        imul        8(%ebp), %edi
        addl        $idt, %edi

        movl        12(%ebp), %edx
        andl        $0x0000FFFF, %edx
        orl         $0x00080000, %edx
        movl        %edx, (%edi)
        addl        $4, %edi

        movl        12(%ebp), %edx
        andl        $0xFFFF0000, %edx
        orl         $0x00008E00, %edx
        movl        %edx, (%edi)

        popl        %ebp
        ret

    .size idt_register_interrupt, . - idt_register_interrupt

