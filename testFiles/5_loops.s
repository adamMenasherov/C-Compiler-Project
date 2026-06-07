   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl $0, -4(%rbp)
	movl $1, -8(%rbp)
.Lloop3:
	movl -8(%rbp), %r11d
	cmpl $5, %r11d
	movl $0, -12(%rbp)
	setle -12(%rbp)
	movl -12(%rbp), %r11d
	cmpl $0, %r11d
	je .Lbreak_loop3
	movl -4(%rbp), %r10d
	movl %r10d, -16(%rbp)
	movl -8(%rbp), %r10d
	addl %r10d, -16(%rbp)
	movl -16(%rbp), %r10d
	movl %r10d, -4(%rbp)
.Lcontinue_loop3:
	incl -8(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -20(%rbp)
	jmp .Lloop3
.Lbreak_loop3:
	movl $10, -24(%rbp)
.Lcontinue_loop4:
	movl -24(%rbp), %r11d
	cmpl $0, %r11d
	movl $0, -28(%rbp)
	setg -28(%rbp)
	movl -28(%rbp), %r11d
	cmpl $0, %r11d
	je .Lbreak_loop4
	movl -24(%rbp), %r10d
	movl %r10d, -32(%rbp)
	subl $3, -32(%rbp)
	movl -32(%rbp), %r10d
	movl %r10d, -24(%rbp)
	jmp .Lcontinue_loop4
.Lbreak_loop4:
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .section .note.GNU-stack,"",@progbits
