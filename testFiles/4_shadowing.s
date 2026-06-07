   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $1, -4(%rbp)
	movl $2, -8(%rbp)
	movl $3, -12(%rbp)
	incl -12(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, -16(%rbp)
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .section .note.GNU-stack,"",@progbits
