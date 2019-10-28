	// void *multiboot_memory_map(void *);

	// Takes a pointer to the multiboot structure given by the bootloader
	// and returns a pointer to the memory map tag, or NULL if no memory
	// map is found.

	.section .text
	.code32
	.global _multiboot_memory_map
	.type _multiboot_memory_map, @function

	.set MB_TAG_MEMORY_MAP, 6
_multiboot_memory_map:
	push	%ebp
	movl	%esp,%ebp
	// 8(%esp) holds the pointer to multiboot structure

	// scratch registers: eax ecx edx
	// preserved registers: ebx esi edi ebp esp

	// save preserved registers
	push	%edi
	push	%esi
	push	%ebx

	// Grab address of MB structure
	movl	8(%ebp),%edi
	// Save the end of the structure
	movl	%edi,%esi
	movl	(%edi),%ecx
	addl	%ecx,%esi

	// Move to the first tag
	addl	$8,%edi
check_tag:
	// Check if we are past the end of the structure
	cmpl	%esi,%edi
	jge	memory_map_not_found

	// Check if this tag is the memory map
	cmpl	$(MB_TAG_MEMORY_MAP),(%edi)
	je	memory_map_found

	// This is not the memory map, size of the tag is saved 4
	// bytes into the tag structure
	movl	4(%edi),%ecx

	// ecx now holds the size of the tag including header fields
	// but not including padding. Tags are padded so that teach one
	// starts at an 8-bytes aligned address.

	// To get the size to the next aligned addres, we need to
	// ecx += (8 - (ecx % 8))
	movl	%ecx,%eax
	xor	%edx,%edx
	movl	$8,%ebx
	divl	%ebx
	// edx now holds ecx % 8; ebx = 8 - (ecx % 8)
	subl	%edx,%ebx
	addl	%ebx,%ecx

	addl	%ecx,%edi
	jmp	check_tag

memory_map_found:
	// Memory map has been found and its address is in edx
	movl	%edi,%eax
	jmp	finish
memory_map_not_found:
	// Memory map has not been found; return 0
	xorl	%eax,%eax
finish:
	// Return value stored in %eax
	pop	%ebx
	pop	%esi
	pop	%edi
	pop	%ebp
	ret

	.size _multiboot_memory_map, . - _multiboot_memory_map
