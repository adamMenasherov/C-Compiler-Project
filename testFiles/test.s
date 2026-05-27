   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $3, %r11d
	cmpl $5, %r11d
	movl $0, -4(%rbp)
	setl -4(%rbp)
	movl -4(%rbp), %r11d
	cmpl $0, %r11d
	je .Lelse_label_0
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	jmp .Lend_label_0
.Lelse_label_0:
	movl $1, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.Lend_label_0:
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .section .note.GNU-stack,"",@progbits
