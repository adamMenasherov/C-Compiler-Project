   .globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $15, -4(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -8(%rbp)
	xorl $5, -8(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl $1, -16(%rbp)
	movl -12(%rbp), %r10d
	orl %r10d, -16(%rbp)
	movl -16(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
   .section .note.GNU-stack,"",@progbits
