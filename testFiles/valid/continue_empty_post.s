   .globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $24, %rsp
	movl $0, -4(%rbp)
	movl $0, -8(%rbp)
.Lloop2:
	movl -8(%rbp), %r11d
	cmpl $10, %r11d
	movl $0, -12(%rbp)
	setl -12(%rbp)
	movl -12(%rbp), %r11d
	cmpl $0, %r11d
	je .Lbreak_loop2
	movl -8(%rbp), %r10d
	movl %r10d, -16(%rbp)
	addl $1, -16(%rbp)
	movl -16(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %eax
	cdq
	movl $2, %r10d
	idivl %r10d
	movl %edx, -20(%rbp)
	movl -20(%rbp), %r11d
	cmpl $0, %r11d
	je .Lend_label_0
	jmp .Lcontinue_loop2
.Lend_label_0:
	movl -4(%rbp), %r10d
	movl %r10d, -24(%rbp)
	movl -8(%rbp), %r10d
	addl %r10d, -24(%rbp)
	movl -24(%rbp), %r10d
	movl %r10d, -4(%rbp)
.Lcontinue_loop2:
	jmp .Lloop2
.Lbreak_loop2:
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret
   .section .note.GNU-stack,"",@progbits
