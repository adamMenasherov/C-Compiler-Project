   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl $0, -4(%rbp)
.Lloop1:
	movl -4(%rbp), %r11d
	cmpl $5, %r11d
	movl $0, -8(%rbp)
	setl -8(%rbp)
	movl -8(%rbp), %r11d
	cmpl $0, %r11d
	je .Lbreak_loop1
	movl foo(%rip), %r10d
	movl %r10d, -12(%rbp)
	addl $1, -12(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, foo(%rip)
.Lcontinue_loop1:
	movl -4(%rbp), %r10d
	movl %r10d, -16(%rbp)
	addl $1, -16(%rbp)
	movl -16(%rbp), %r10d
	movl %r10d, -4(%rbp)
	jmp .Lloop1
.Lbreak_loop1:
	movl foo(%rip), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .bss
   .align 4
   .globl foo
foo:
	.zero 4
   .section .note.GNU-stack,"",@progbits
