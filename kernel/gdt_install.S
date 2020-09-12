    // void gdt_install(gdtr_t * gdt, uint16_t cs, uint16_t ds);
    .global gdt_install
gdt_install:
    // rdi: address of gdtr
    // rsi: CS
    // rdx: DS
    push %rbp
    mov %rsp,%rbp
    lgdt (%rdi)

    movabsq $cs_set,%rax
    push %rsi
    push %rax
    lretq
cs_set:

	// Now clear the data segment descriptor
	movw	$0,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	// Set data segment descriptor
	movw	%dx,%ax
	movw	%ax,%ss
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs

    pop %rbp
    ret
    .size gdt_install, . - gdt_install
