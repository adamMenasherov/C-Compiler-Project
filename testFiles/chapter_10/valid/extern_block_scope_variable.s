   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $1, -4(%rbp)
	movl $0, -8(%rbp)
	movl -4(%rbp), %r11d
	cmpl $0, %r11d
	je .Lend_label_0
	movl foo(%rip), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.Lend_label_0:
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .data
   .globl foo
foo:
	.long 3
   .section .note.GNU-stack,"",@progbits
