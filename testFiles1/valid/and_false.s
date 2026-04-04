   .globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $20, %rsp
	movl $10, %r11d
	cmpl $0, %r11d
	je .Lfalse_label_0
	movl $0, %r11d
	cmpl $0, %r11d
	je .Lfalse_label_0
	movl $1, -4(%rbp)
	jmp .Lend_label_0
.Lfalse_label_0:
	movl $0, -4(%rbp)
.Lend_label_0:
	movl $0, %r11d
	cmpl $0, %r11d
	je .Lfalse_label_1
	movl $4, %r11d
	cmpl $0, %r11d
	je .Lfalse_label_1
	movl $1, -8(%rbp)
	jmp .Lend_label_1
.Lfalse_label_1:
	movl $0, -8(%rbp)
.Lend_label_1:
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl -8(%rbp), %r10d
	addl %r10d, -12(%rbp)
	movl $0, %r11d
	cmpl $0, %r11d
	je .Lfalse_label_2
	movl $0, %r11d
	cmpl $0, %r11d
	je .Lfalse_label_2
	movl $1, -16(%rbp)
	jmp .Lend_label_2
.Lfalse_label_2:
	movl $0, -16(%rbp)
.Lend_label_2:
	movl -12(%rbp), %r10d
	movl %r10d, -20(%rbp)
	movl -16(%rbp), %r10d
	addl %r10d, -20(%rbp)
	movl -20(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
   .section .note.GNU-stack,"",@progbits
