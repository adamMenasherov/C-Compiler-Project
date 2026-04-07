   .globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $44, %rsp
	movl $10, -4(%rbp)
	movl $20, -8(%rbp)
	movl $20, -12(%rbp)
	negl -12(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, -8(%rbp)
.Lloop2:
	movl -8(%rbp), %r11d
	cmpl $0, %r11d
	movl $0, -16(%rbp)
	setl -16(%rbp)
	movl -16(%rbp), %r11d
	cmpl $0, %r11d
	je .Lbreak_loop2
	movl -4(%rbp), %r10d
	movl %r10d, -20(%rbp)
	subl $1, -20(%rbp)
	movl -20(%rbp), %r10d
	movl %r10d, -4(%rbp)
	movl -4(%rbp), %r11d
	cmpl $0, %r11d
	movl $0, -24(%rbp)
	setle -24(%rbp)
	movl -24(%rbp), %r11d
	cmpl $0, %r11d
	je .Lend_label_0
	jmp .Lbreak_loop2
.Lend_label_0:
.Lcontinue_loop2:
	movl -8(%rbp), %r10d
	movl %r10d, -28(%rbp)
	addl $1, -28(%rbp)
	movl -28(%rbp), %r10d
	movl %r10d, -8(%rbp)
	jmp .Lloop2
.Lbreak_loop2:
	movl -4(%rbp), %r11d
	cmpl $0, %r11d
	movl $0, -32(%rbp)
	sete -32(%rbp)
	movl -32(%rbp), %r11d
	cmpl $0, %r11d
	je .Lfalse_label_0
	movl $11, -36(%rbp)
	negl -36(%rbp)
	movl -36(%rbp), %r10d
	movl -8(%rbp), %r11d
	cmpl %r10d, %r11d
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
