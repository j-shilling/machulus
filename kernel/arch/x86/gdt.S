.section .rodata, "", @progbits
    
    .type gdt, @object
    gdt:

        .8byte      0   /* GDT must start with a NULL entry */
        
        /* Actual GDT entries. Segmentation is not used so the base is always 0
           and limit always 0xFFFFFFFF */

        /* Kernel Code */
        .2byte      0xFFFF      // limit[0-15]
        .2byte      0x0         // base[0-15]
        .byte       0x0         // base[16-23]
        .byte       0b10011010  // access byte
        .byte       0b11001111  // flags + limit[16-19]
        .byte       0x0         // base[24-31]

        /* Kernel Data */
        .2byte      0xFFFF      // limit[0-15]
        .2byte      0x0         // base[0-15]
        .byte       0x0         // base[16-23]
        .byte       0b10010010  // access byte
        .byte       0b11001111  // flags + limit[16-19]
        .byte       0x0         // base[24-31]

        /* Server Code */
        .2byte      0xFFFF      // limit[0-15]
        .2byte      0x0         // base[0-15]
        .byte       0x0         // base[16-23]
        .byte       0b10111010  // access byte
        .byte       0b11001111  // flags + limit[16-19]
        .byte       0x0         // base[24-31]

        /* Server Data */
        .2byte      0xFFFF      // limit[0-15]
        .2byte      0x0         // base[0-15]
        .byte       0x0         // base[16-23]
        .byte       0b10110010  // access byte
        .byte       0b11001111  // flags + limit[16-19]
        .byte       0x0         // base[24-31]

        /* Module Code */
        .2byte      0xFFFF      // limit[0-15]
        .2byte      0x0         // base[0-15]
        .byte       0x0         // base[16-23]
        .byte       0b11011010  // access byte
        .byte       0b11001111  // flags + limit[16-19]
        .byte       0x0         // base[24-31]

        /* Module Data */
        .2byte      0xFFFF      // limit[0-15]
        .2byte      0x0         // base[0-15]
        .byte       0x0         // base[16-23]
        .byte       0b11010010  // access byte
        .byte       0b11001111  // flags + limit[16-19]
        .byte       0x0         // base[24-31]

        /* User Code */
        .2byte      0xFFFF      // limit[0-15]
        .2byte      0x0         // base[0-15]
        .byte       0x0         // base[16-23]
        .byte       0b11111010  // access byte
        .byte       0b11001111  // flags + limit[16-19]
        .byte       0x0         // base[24-31]

        /* User Data */
        .2byte      0xFFFF      // limit[0-15]
        .2byte      0x0         // base[0-15]
        .byte       0x0         // base[16-23]
        .byte       0b11110010  // access byte
        .byte       0b11001111  // flags + limit[16-19]
        .byte       0x0         // base[24-31]

    gdt_end:
    .size gdt, . - gdt

    .type gdt_descriptor, @object
    gdt_descriptor:

        .2byte      gdt_end - gdt - 1
        .4byte      gdt

    .size gdt_descriptor, . - gdt_descriptor

.section .text

    .type gdt_init, @function
    .global gdt_init
    gdt_init:
    
        lgdt        gdt_descriptor        
        ret
        
    .size gdt_init, . - gdt_init

