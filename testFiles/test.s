   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	movl $5, 4(%rbp)
	leaq 4(%rbp), %r11
	movq %r11, 16(%rbp)
	movq 16(%rbp), %r10
	movq %r10, 24(%rbp)
	movq 24(%rbp), %r10
	movq 0(%r10), %r10
	movq %r10, 28(%rbp)
	movslq 28(%rbp), %r11
	movq %r11, 40(%rbp)
	movq 40(%rbp), %rax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .section .note.GNU-stack,"",@progbits
