   .globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $24, %rsp
	movl $1, -4(%rbp)
	movl $4, -8(%rbp)
	pushq %rcx
	movl -4(%rbp), %ecx
	sall %cl, -8(%rbp)
	popq %rcx
	movl -8(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl -4(%rbp), %r11d
	cmpl $1, %r11d
	movl $0, -16(%rbp)
	sete -16(%rbp)
	movl -16(%rbp), %r11d
	cmpl $0, %r11d
	je .Lfalse_label_0
	movl -12(%rbp), %r11d
	cmpl $8, %r11d
	movl $0, -20(%rbp)
	sete -20(%rbp)
	movl -20(%rbp), %r11d
	cmpl $0, %r11d
	je .Lfalse_label_0
	movl $1, -24(%rbp)
	jmp .Lend_label_0
.Lfalse_label_0:
	movl $0, -24(%rbp)
.Lend_label_0:
	movl -24(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
   .section .note.GNU-stack,"",@progbits
