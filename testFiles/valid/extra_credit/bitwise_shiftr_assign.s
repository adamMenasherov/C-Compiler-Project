   .globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $12, %rsp
	movl $1234, -4(%rbp)
	movl $0, -8(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	sarl $4, -12(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
   .section .note.GNU-stack,"",@progbits
