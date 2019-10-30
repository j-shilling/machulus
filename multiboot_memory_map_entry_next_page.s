	// void *multiboot_memory_map_entry_next_page(void *);

	// Takes a pointer to an entry in the memory map. This entry is
	// assumed to point to a block of available ram. Returns a pointer
	// to the next available page frame or 0 if no more is available
	// in this entry. This function caches it's previous return value,
	// so it's not stateless.

	.extern _KERNEL_END

	.section .data
	// static variable to store the last return value
previous_return:
	.4byte	0x0

	.section .text
	.code32
	.global _multiboot_memory_map_entry_next_page
	.type _multiboot_memory_map_entry_next_page, @function
	.set KERNEL_OFFSET, 0xffffffff80000000
_multiboot_memory_map_entry_next_page:
	push	%ebp
	movl	%esp,%ebp

	// scratch registers: eax ecx edx
	// preserved registers: ebx esi edi ebp esp
	push	%ebx
	push	%edi
	push	%esi

	// Grab address of the entry structure
	movl	8(%ebp),%esi

	// Check if we have a saved return value
	movl	$(previous_return - KERNEL_OFFSET),%ebx
	movl	(%ebx),%edi
	cmpl	$0,%edi
	jne	increment_to_next_page

	// %edi is 0 and needs to get initialized to the beginning
	// of this memory block. Base address is held as a uint64_t
	// starting at (%esi).

	// First we need to make sure that the base address fits in
	// our 32-bit address space by ensuring that the higher bits
	// are all 0.
	movl	4(%esi),%edi
	cmpl	$0,%edi
	jne	not_found

	// The lower 32-bits are the base address for this region.
	movl	(%esi),%edi
	// We only want to use memory after the kernel, so edi = max(edi,KERNEL_END)
	movl	$(_KERNEL_END - KERNEL_OFFSET),%ecx
	cmpl	%edi,%ecx
	jle	page_align_edi
	movl	%ecx,%edi
page_align_edi:
	// Page align the base address edi += (0x1000 - (edi % 0x1000))
	xorl	%edx,%edx
	movl	%edi,%eax
	movl	$0x1000,%ecx
	divl	%ecx
	subl	%edx,%ecx
	addl	%ecx,%edi
	jmp	find_end_of_block

increment_to_next_page:
	// %edi hold the previously returned page. Increment to the
	// next one
	addl	$0x1000,%edi

find_end_of_block:
	// We need to find the end of this block of memory that is
	// usable in 32-bit address space. The length is stored as
	// a uint64_t starting at esi + 8.

	// get the higher bits
	movl	12(%esi),%edx
	// if edx != 0, then this block moves past the end of 32-bit
	// address space
	cmpl	$0,%edx
	jne	truncate_block

	// get the lower bits
	movl	8(%esi),%edx

	// Add block length to the base address to get the end of the
	// block
	movl	(%esi),%ecx
	addl	%ecx,%edx
	// But if the length was too big for our 32-bit register we
	// need to trucate the block. If the carry bit is not set,
	// we can skip ahead
	jnc	check_page
truncate_block:
	// If we get here then the block extends past the end of 32-bit
	// address space. We set the end of the block to just be the
	// last possible address
	movl	$0xffffffff,%edx
check_page:
	// Get the last possible page with edx & ~(0xfff)
	andl	$(~0xfff),%edx
	// if edi <= edx, then we have found a usable page
	cmpl	%edx,%edi
	jle	finish
not_found:
	// return 0
	movl	$0,%edi
finish:
	// Save edi into previous return value and return it. ebx
	// should still hold the address of previous_return.
	movl	%edi,(%ebx)
	movl	%edi,%eax

	pop	%esi
	pop	%edi
	pop	%ebx
	pop	%ebp
	ret

	.size _multiboot_memory_map_entry_next_page, . - _multiboot_memory_map_entry_next_page
