   .text
foo:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $42, %eax
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
	movl -4(%rbp), %r11d
	cmpl $1, %r11d
	movl $0, -8(%rbp)
	sete -8(%rbp)
	movl -8(%rbp), %r11d
	cmpl $0, %r11d
	jne .Lcase_1_1
	movl -4(%rbp), %r11d
	cmpl $6, %r11d
	movl $0, -12(%rbp)
	sete -12(%rbp)
	movl -12(%rbp), %r11d
	cmpl $0, %r11d
	jne .Lcase_6_3
	jmp .Ldefault_label_0
.Lcase_1_1:
	call foo
	movl %eax, -16(%rbp)
	movl -16(%rbp), %r10d
	movl %r10d, -20(%rbp)
	movl -4(%rbp), %r10d
	movl -20(%rbp), %r11d
	imull %r10d, %r11d
	movl %r11d, -20(%rbp)
	movl -20(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	jmp .Lend_label_0
.Lcase_6_3:
	call foo
	movl %eax, -24(%rbp)
	movl -24(%rbp), %r10d
	movl %r10d, -28(%rbp)
	movl -4(%rbp), %r10d
	addl %r10d, -28(%rbp)
	movl -28(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	jmp .Lend_label_0
.Ldefault_label_0:
	call foo
	movl %eax, -32(%rbp)
	movl -32(%rbp), %r10d
	movl %r10d, -36(%rbp)
	movl -36(%rbp), %r11d
	imull $2, %r11d
	movl %r11d, -36(%rbp)
	movl -36(%rbp), %r10d
	movl %r10d, -40(%rbp)
	movl -4(%rbp), %r10d
	addl %r10d, -40(%rbp)
	movl -40(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.Lend_label_0:
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .data
a:
	.long 10
   .data
b:
	.long 10
   .section .note.GNU-stack,"",@progbits
