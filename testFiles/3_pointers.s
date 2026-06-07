   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl $42, -4(%rbp)
	leaq -4(%rbp), %r11
	movq %r11, -16(%rbp)
	movq -16(%rbp), %r10
	movq %r10, -24(%rbp)
	movq -24(%rbp), %r11
	movl $10, 0(%r11)
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .section .note.GNU-stack,"",@progbits
