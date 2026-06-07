   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $2, -4(%rbp)
	movl -4(%rbp), %r11d
	imull $3, %r11d
	movl %r11d, -4(%rbp)
	movl $1, -8(%rbp)
	movl -4(%rbp), %r10d
	addl %r10d, -8(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl -12(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .section .note.GNU-stack,"",@progbits
