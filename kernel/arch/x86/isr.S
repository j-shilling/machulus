
.section .data
msg: .ascii "Hello, World\n"

.section .text
    .global test_handler
    .type test_handler, @function
    test_handler:
        pushal

        push $msg
        call printf
        addl $4, %esp

        popal
        iret
    .size test_handler, . - test_handler