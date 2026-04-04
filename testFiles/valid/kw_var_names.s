   .globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $12, %rsp
	movl $3, -4(%rbp)
	movl $2, -8(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl -8(%rbp), %r10d
	addl %r10d, -12(%rbp)
	movl -12(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
   .section .note.GNU-stack,"",@progbits
