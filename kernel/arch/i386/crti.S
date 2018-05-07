.section .init
.global init
.type init, @function
init:
	push %ebp
	movl %esp, %ebp

	/* gcc will put the contents of crtbegin.o's .init here. */

.section .fini
.global fini
.type fini, @function
fini:
	push %ebp
	movl %esp, %ebp

	/* gcc will put the contents of crtbegin.o's .fini here. */
