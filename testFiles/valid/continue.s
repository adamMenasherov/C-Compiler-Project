   .globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $44, %rsp
	movl $0, -4(%rbp)
	movl $0, -8(%rbp)
.Lloop3:
	movl -8(%rbp), %r11d
	cmpl $10, %r11d
	movl $0, -12(%rbp)
	setle -12(%rbp)
	movl -12(%rbp), %r11d
	cmpl $0, %r11d
	je .Lbreak_loop3
	movl -8(%rbp), %r10d
	movl %r10d, -16(%rbp)
	movl -8(%rbp), %eax
	cdq
	movl $2, %r10d
	idivl %r10d
	movl %edx, -20(%rbp)
	movl -20(%rbp), %r11d
	cmpl $0, %r11d
	movl $0, -24(%rbp)
	sete -24(%rbp)
	movl -24(%rbp), %r11d
	cmpl $0, %r11d
	je .Lend_label_0
	jmp .Lcontinue_loop3
.Lend_label_0:
	movl -4(%rbp), %r10d
	movl %r10d, -28(%rbp)
	addl $1, -28(%rbp)
	movl -28(%rbp), %r10d
	movl %r10d, -4(%rbp)
.Lcontinue_loop3:
	movl -8(%rbp), %r10d
	movl %r10d, -32(%rbp)
	addl $1, -32(%rbp)
	movl -32(%rbp), %r10d
	movl %r10d, -8(%rbp)
	jmp .Lloop3
.Lbreak_loop3:
	movl -4(%rbp), %r11d
	cmpl $5, %r11d
	movl $0, -36(%rbp)
	sete -36(%rbp)
	movl -36(%rbp), %r11d
	cmpl $0, %r11d
	je .Lfalse_label_0
	movl -16(%rbp), %r11d
	cmpl $10, %r11d
	movl $0, -40(%rbp)
	sete -40(%rbp)
	movl -40(%rbp), %r11d
	cmpl $0, %r11d
	je .Lfalse_label_0
	movl $1, -44(%rbp)
	jmp .Lend_label_1
.Lfalse_label_0:
	movl $0, -44(%rbp)
.Lend_label_1:
	movl -44(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret
   .section .note.GNU-stack,"",@progbits
