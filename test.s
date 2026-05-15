   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $5, -4(%rbp)
	movl -4(%rbp), %r11d
	cmpl $5, %r11d
	movl $0, -8(%rbp)
	sete -8(%rbp)
	movl -8(%rbp), %r11d
	cmpl $0, %r11d
	jne .Lcase_5_1
	jmp .Ldefault_label_0
.Lcase_5_1:
	movl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl foo(%rip), %r10d
	addl %r10d, -12(%rbp)
	movl -12(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	jmp .Lend_label_0
.Ldefault_label_0:
	movl foo(%rip), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.Lend_label_0:
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .data
foo:
	.long 5
   .section .note.GNU-stack,"",@progbits
