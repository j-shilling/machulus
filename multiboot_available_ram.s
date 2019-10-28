	// void *multiboot_available_ram(void *)

	// Takes a pointer to the memory map take from the multiboot structure
	// and returns a pointer to the next entry indicating available RAM. Each
	// return value is cached in a static variable, so this function is not
	// stateless.

	.section .data
	// static variable to store last return value
previous_entry:
	.4byte	0x0

	.section .text
	.code32
	.global _multiboot_available_ram
	.type _mutliboot_available_ram, @function

	.set MB_AVAILABLE_RAM, 1
	.set KERNEL_OFFSET, 0xffffffff80000000
_multiboot_available_ram:
	push	%ebp
	movl	%esp,%ebp

	// scratch registers: eax ecx edx
	// preserved registers: ebx esi edi ebp esp
	push %edi
	push %esi
	push %ebx

	// Grab the memory map from the stack
	movl	8(%ebp),%edi
	// Get the size of an entry
	movl	8(%edi),%ebx
	// Get the end of the memory map
	movl	4(%edi),%ecx
	addl	%edi,%ecx

	// Grap the previous entry, or the first one
	movl	$(previous_entry - KERNEL_OFFSET),%eax
	movl	(%eax),%esi
	cmpl	$0,%esi
	jne	next_entry

	// Get the first entry from the memory map
	movl	%edi,%esi
	addl	$16,%esi

check_entry:
	// Check whether we are still in the memory map
	cmpl	%ecx,%esi
	jge	available_ram_not_found

	// edx points to an entry from the memeory map

	// Get the entry's type
	movl	16(%esi),%edx
	cmpl	$(MB_AVAILABLE_RAM),%edx
	je	available_ram_found
next_entry:
	// Move to next entry
	addl	%ebx,%esi
	jmp	check_entry

available_ram_not_found:
	xorl	%esi,%esi
available_ram_found:
	movl	%esi,(%eax)
	movl	%esi,%eax

	pop	%ebx
	pop	%esi
	pop	%edi
	pop	%ebp

	ret

	.size _multiboot_available_ram, . - _multiboot_available_ram
