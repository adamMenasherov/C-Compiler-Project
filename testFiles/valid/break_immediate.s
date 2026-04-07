   .globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $4, %rsp
	movl $10, -4(%rbp)
.Lcontinue_loop1:
	movl $1, -4(%rbp)
	movl -4(%rbp), %r11d
	cmpl $0, %r11d
	je .Lbreak_loop1
	jmp .Lcontinue_loop1
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
