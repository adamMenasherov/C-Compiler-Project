   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $64, %rsp
	movl $1, -32(%rbp)
	movl $2, -28(%rbp)
	movl $3, -24(%rbp)
	movl $4, -20(%rbp)
	movl $5, -16(%rbp)
	leaq -32(%rbp), %r11
	movq %r11, -40(%rbp)
	movl $2, %r10d
	movslq %r10d, %r11
	movq %r11, -48(%rbp)
	movq -40(%rbp), %rax
	movq -48(%rbp), %rdx
	leaq (%rax,%rdx,4), %r11
	movq %r11, -56(%rbp)
	movq -56(%rbp), %r10
	movl 0(%r10), %r10d
	movl %r10d, -60(%rbp)
	movl -60(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .section .note.GNU-stack,"",@progbits
