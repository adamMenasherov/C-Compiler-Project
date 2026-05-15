   .text
foo:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl %edi, -4(%rbp)
	incl a.1(%rip)
	movl a.1(%rip), %r10d
	movl %r10d, -8(%rbp)
	movl a.1(%rip), %r10d
	movl %r10d, -12(%rbp)
	movl -4(%rbp), %r10d
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
	subq $16, %rsp
	movl a.2(%rip), %r10d
	movl %r10d, -4(%rbp)
	movl -4(%rbp), %edi
	call foo
	movl %eax, -8(%rbp)
	movl -8(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .data
a.1:
	.long 5
   .data
a.2:
	.long 4
   .section .note.GNU-stack,"",@progbits
