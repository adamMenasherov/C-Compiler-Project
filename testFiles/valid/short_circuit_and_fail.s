   .globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $8, %rsp
	movl $0, -4(%rbp)
	movl $0, %r11d
	cmpl $0, %r11d
	je .Lfalse_label_0
	movl $5, -4(%rbp)
	movl -4(%rbp), %r11d
	cmpl $0, %r11d
	je .Lfalse_label_0
	movl $1, -8(%rbp)
	jmp .Lend_label_0
.Lfalse_label_0:
	movl $0, -8(%rbp)
.Lend_label_0:
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
   .section .note.GNU-stack,"",@progbits
