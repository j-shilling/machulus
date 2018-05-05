.section .init
	/* this will come after gcc crtend .init insertion */
	popl %ebp
	ret

.section .fini
	/* this will come after gcc crtend .fini insertion */
	popl %ebp
	ret
