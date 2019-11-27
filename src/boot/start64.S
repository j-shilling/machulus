	.section .text
	.code64
	.global _start64
	.type _start64, @function
_start64:

	cli
1:	hlt
	jmp 1b
	.size _start64, . - _start64
