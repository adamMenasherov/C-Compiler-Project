   .text
foo:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl %edi, -4(%rbp)
	movl %esi, -8(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl -8(%rbp), %r10d
	addl %r10d, -12(%rbp)
	movl -12(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	movl $5, -4(%rbp)
	movl $10, -8(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -16(%rbp)
	movl -12(%rbp), %edi
	movl -16(%rbp), %esi
	call foo
	movl %eax, -20(%rbp)
	movl -20(%rbp), %r10d
	movl %r10d, -24(%rbp)
	movl $65, -28(%rbp)
	movl -28(%rbp), %edi
	call putchar@PLT
	movl %eax, -32(%rbp)
	movl -24(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .section .note.GNU-stack,"",@progbits
