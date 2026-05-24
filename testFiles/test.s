   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl $10, -4(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl -16(%rbp), %r10d
	movl %r10d, -20(%rbp)
	movl -12(%rbp), %r10d
	addl %r10d, -20(%rbp)
	movl -24(%rbp), %r10d
	movl %r10d, -28(%rbp)
	movl -28(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .data
val:
	.long 5
   .section .note.GNU-stack,"",@progbits
