   .globl print_alphabet
   .text
print_alphabet:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl count.1(%rip), %r10d
	movl %r10d, -4(%rbp)
	addl $65, -4(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %edi
	call putchar@PLT
	movl %eax, -12(%rbp)
	movl count.1(%rip), %r10d
	movl %r10d, -16(%rbp)
	addl $1, -16(%rbp)
	movl -16(%rbp), %r10d
	movl %r10d, count.1(%rip)
	movl count.1(%rip), %r11d
	cmpl $26, %r11d
	movl $0, -20(%rbp)
	setl -20(%rbp)
	movl -20(%rbp), %r11d
	cmpl $0, %r11d
	je .Lend_label_0
	call print_alphabet
	movl %eax, -24(%rbp)
.Lend_label_0:
	movl count.1(%rip), %eax
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
	subq $16, %rsp
	call print_alphabet
	movl %eax, -4(%rbp)
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .bss
   .align 4
count.1:
	.zero 4
   .section .note.GNU-stack,"",@progbits
