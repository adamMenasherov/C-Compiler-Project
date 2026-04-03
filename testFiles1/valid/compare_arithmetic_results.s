   .globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $20, %rsp
	movl $2, -4(%rbp)
	notl -4(%rbp)
	movl $2, -8(%rbp)
	negl -8(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl -8(%rbp), %r10d
	movl -12(%rbp), %r11d
	imull %r10d, %r11d
	movl %r11d, -12(%rbp)
	movl $1, -16(%rbp)
	addl $5, -16(%rbp)
	movl -16(%rbp), %r10d
	movl -12(%rbp), %r11d
	cmpl %r10d, %r11d
	movl $0, -20(%rbp)
	sete -20(%rbp)
	movl -20(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
   .section .note.GNU-stack,"",@progbits
