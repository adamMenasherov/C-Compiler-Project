   .globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $12, %rsp
	movl $1, -4(%rbp)
.Lloop1:
	movl -4(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl -8(%rbp), %r11d
	imull $2, %r11d
	movl %r11d, -8(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -4(%rbp)
.Lcontinue_loop1:
	movl -4(%rbp), %r11d
	cmpl $11, %r11d
	movl $0, -12(%rbp)
	setl -12(%rbp)
	movl -12(%rbp), %r11d
	cmpl $0, %r11d
	jne .Lloop1
.Lbreak_loop1:
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret
   .section .note.GNU-stack,"",@progbits
