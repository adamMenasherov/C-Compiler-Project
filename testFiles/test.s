   .globl foo
   .text
foo:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl %edi, -4(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -8(%rbp)
	addl $53, -8(%rbp)
	movslq -8(%rbp), %r11
	movq %r11, -16(%rbp)
	movq -16(%rbp), %rax
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
	subq $32, %rsp
	movl $2, %r10d
	cvtsi2sdl %r10d, %xmm15
	movsd %xmm15, -8(%rbp)
	movsd .LC0(%rip), %xmm15
	movsd %xmm15, -16(%rbp)
	movsd -8(%rbp), %xmm15
	movsd -16(%rbp), %xmm14
	addsd %xmm15, %xmm14
	movsd %xmm14, -16(%rbp)
	movsd .LC1(%rip), %xmm15
	movsd -16(%rbp), %xmm14
	comisd %xmm15, %xmm14
	jae .Lelse_label_0
	cvttsd2siq -16(%rbp), %r11
	movq %r11, -24(%rbp)
	jmp .Lend_label_0
.Lelse_label_0:
	movsd -16(%rbp), %xmm1
	subsd .LC1(%rip), %xmm1
	cvttsd2siq %xmm1, %r11
	movq %r11, -24(%rbp)
	movq $-9223372036854775808, %rax
	addq %rax, -24(%rbp)
.Lend_label_0:
	movq -24(%rbp), %rax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .section .rodata
   .align 8
.LC1:
	.quad 4890909195324358656
   .section .rodata
   .align 8
.LC0:
	.quad 4617315517961601024
   .section .note.GNU-stack,"",@progbits
