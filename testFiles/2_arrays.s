   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $224, %rsp
	movl $2, -32(%rbp)
	movl $4, -28(%rbp)
	movl $6, -24(%rbp)
	movl $8, -20(%rbp)
	movl $10, -16(%rbp)
	movl $12, -12(%rbp)
	leaq -32(%rbp), %r11
	movq %r11, -40(%rbp)
	movl $0, %r10d
	movslq %r10d, %r11
	movq %r11, -48(%rbp)
	movq -40(%rbp), %rax
	movq -48(%rbp), %rdx
	leaq (%rax,%rdx,8), %r11
	movq %r11, -56(%rbp)
	movl $0, %r10d
	movslq %r10d, %r11
	movq %r11, -64(%rbp)
	movq -56(%rbp), %rax
	movq -64(%rbp), %rdx
	leaq (%rax,%rdx,4), %r11
	movq %r11, -72(%rbp)
	movq -72(%rbp), %r10
	movl 0(%r10), %r10d
	movl %r10d, -76(%rbp)
	movl -76(%rbp), %r10d
	movl %r10d, -80(%rbp)
	leaq -32(%rbp), %r11
	movq %r11, -88(%rbp)
	movl $1, %r10d
	movslq %r10d, %r11
	movq %r11, -96(%rbp)
	movq -88(%rbp), %rax
	movq -96(%rbp), %rdx
	leaq (%rax,%rdx,8), %r11
	movq %r11, -104(%rbp)
	movl $1, %r10d
	movslq %r10d, %r11
	movq %r11, -112(%rbp)
	movq -104(%rbp), %rax
	movq -112(%rbp), %rdx
	leaq (%rax,%rdx,4), %r11
	movq %r11, -120(%rbp)
	movq -120(%rbp), %r10
	movl 0(%r10), %r10d
	movl %r10d, -124(%rbp)
	movl -124(%rbp), %r10d
	movl %r10d, -128(%rbp)
	leaq -32(%rbp), %r11
	movq %r11, -136(%rbp)
	movl $1, %r10d
	movslq %r10d, %r11
	movq %r11, -144(%rbp)
	movq -136(%rbp), %rax
	movq -144(%rbp), %rdx
	leaq (%rax,%rdx,8), %r11
	movq %r11, -152(%rbp)
	movl $0, %r10d
	movslq %r10d, %r11
	movq %r11, -160(%rbp)
	movq -152(%rbp), %rax
	movq -160(%rbp), %rdx
	leaq (%rax,%rdx,4), %r11
	movq %r11, -168(%rbp)
	movl -80(%rbp), %r10d
	movl %r10d, -172(%rbp)
	movl -128(%rbp), %r10d
	addl %r10d, -172(%rbp)
	movq -168(%rbp), %r11
	movl -172(%rbp), %r10d
	movl %r10d, 0(%r11)
	leaq -32(%rbp), %r11
	movq %r11, -184(%rbp)
	movl $1, %r10d
	movslq %r10d, %r11
	movq %r11, -192(%rbp)
	movq -184(%rbp), %rax
	movq -192(%rbp), %rdx
	leaq (%rax,%rdx,8), %r11
	movq %r11, -200(%rbp)
	movl $0, %r10d
	movslq %r10d, %r11
	movq %r11, -208(%rbp)
	movq -200(%rbp), %rax
	movq -208(%rbp), %rdx
	leaq (%rax,%rdx,4), %r11
	movq %r11, -216(%rbp)
	movq -216(%rbp), %r10
	movl 0(%r10), %r10d
	movl %r10d, -220(%rbp)
	movl -220(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .section .note.GNU-stack,"",@progbits
