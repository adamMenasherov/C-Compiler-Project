   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $224, %rsp
	movsd zero(%rip), %xmm15
	movsd %xmm15, -8(%rbp)
	movsd .LC0(%rip), %xmm15
	movsd -8(%rbp), %xmm14
	xorpd %xmm15, %xmm14
	movsd %xmm14, -8(%rbp)
	movsd -8(%rbp), %xmm15
	movsd %xmm15, -16(%rbp)
	movl $0, %r10d
	cvtsi2sdl %r10d, %xmm15
	movsd %xmm15, -24(%rbp)
	movsd -24(%rbp), %xmm15
	movsd -16(%rbp), %xmm14
	comisd %xmm15, %xmm14
	movl $0, -28(%rbp)
	setne -28(%rbp)
	movl -28(%rbp), %r11d
	cmpl $0, %r11d
	je .Lend_label_0
	movl $1, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.Lend_label_0:
	movl $1, %r10d
	cvtsi2sdl %r10d, %xmm15
	movsd %xmm15, -40(%rbp)
	movsd -40(%rbp), %xmm15
	movsd %xmm15, -48(%rbp)
	divsd -16(%rbp), %xmm15
	movsd .LC1(%rip), %xmm15
	movsd %xmm15, -56(%rbp)
	movsd .LC0(%rip), %xmm15
	movsd -56(%rbp), %xmm14
	xorpd %xmm15, %xmm14
	movsd %xmm14, -56(%rbp)
	movsd -56(%rbp), %xmm15
	movsd -48(%rbp), %xmm14
	comisd %xmm15, %xmm14
	movl $0, -60(%rbp)
	setne -60(%rbp)
	movl -60(%rbp), %r11d
	cmpl $0, %r11d
	je .Lend_label_1
	movl $2, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.Lend_label_1:
	movl $10, -64(%rbp)
	negl -64(%rbp)
	cvtsi2sdl -64(%rbp), %xmm15
	movsd %xmm15, -72(%rbp)
	movsd -72(%rbp), %xmm15
	movsd %xmm15, -80(%rbp)
	divsd -16(%rbp), %xmm15
	movsd .LC1(%rip), %xmm15
	movsd -80(%rbp), %xmm14
	comisd %xmm15, %xmm14
	movl $0, -84(%rbp)
	setne -84(%rbp)
	movl -84(%rbp), %r11d
	cmpl $0, %r11d
	je .Lend_label_2
	movl $3, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.Lend_label_2:
	movl $0, -88(%rbp)
	xorpd %xmm0, %xmm0
	comisd -16(%rbp), %xmm0
	je .Lfalse_label_0
	movl $1, -88(%rbp)
	cvtsi2sdl -88(%rbp), %xmm15
	movsd %xmm15, -96(%rbp)
	xorpd %xmm0, %xmm0
	comisd -96(%rbp), %xmm0
	je .Lfalse_label_0
	movl $1, -104(%rbp)
	jmp .Lend_label_3
.Lfalse_label_0:
	movl $0, -104(%rbp)
.Lend_label_3:
	movl -88(%rbp), %r11d
	cmpl $0, %r11d
	je .Lend_label_4
	movl $4, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.Lend_label_4:
	xorpd %xmm0, %xmm0
	comisd -16(%rbp), %xmm0
	je .Lend_label_5
	movl $5, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.Lend_label_5:
	movsd .LC2(%rip), %xmm15
	movsd %xmm15, -112(%rbp)
	movsd .LC0(%rip), %xmm15
	movsd -112(%rbp), %xmm14
	xorpd %xmm15, %xmm14
	movsd %xmm14, -112(%rbp)
	movsd -112(%rbp), %xmm15
	movsd zero(%rip), %xmm14
	comisd %xmm15, %xmm14
	movl $0, -116(%rbp)
	setne -116(%rbp)
	movl -116(%rbp), %r11d
	cmpl $0, %r11d
	je .Lend_label_6
	movl $6, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.Lend_label_6:
	movsd .LC3(%rip), %xmm15
	movsd %xmm15, -128(%rbp)
	movsd .LC2(%rip), %xmm15
	movsd %xmm15, -136(%rbp)
	movsd .LC0(%rip), %xmm15
	movsd -136(%rbp), %xmm14
	xorpd %xmm15, %xmm14
	movsd %xmm14, -136(%rbp)
	movsd -136(%rbp), %xmm15
	movsd %xmm15, -144(%rbp)
	movsd -128(%rbp), %xmm0
	movsd -144(%rbp), %xmm1
	call copysign@PLT
	movsd %xmm0, -152(%rbp)
	movsd -152(%rbp), %xmm15
	movsd %xmm15, -160(%rbp)
	movsd .LC4(%rip), %xmm15
	movsd %xmm15, -168(%rbp)
	movsd .LC0(%rip), %xmm15
	movsd -168(%rbp), %xmm14
	xorpd %xmm15, %xmm14
	movsd %xmm14, -168(%rbp)
	movsd -168(%rbp), %xmm15
	movsd %xmm15, -176(%rbp)
	movsd .LC2(%rip), %xmm15
	movsd %xmm15, -184(%rbp)
	movsd -176(%rbp), %xmm0
	movsd -184(%rbp), %xmm1
	call copysign@PLT
	movsd %xmm0, -192(%rbp)
	movsd -192(%rbp), %xmm15
	movsd %xmm15, -200(%rbp)
	movsd .LC3(%rip), %xmm15
	movsd %xmm15, -208(%rbp)
	movsd .LC0(%rip), %xmm15
	movsd -208(%rbp), %xmm14
	xorpd %xmm15, %xmm14
	movsd %xmm14, -208(%rbp)
	movsd -208(%rbp), %xmm15
	movsd -160(%rbp), %xmm14
	comisd %xmm15, %xmm14
	movl $0, -212(%rbp)
	setne -212(%rbp)
	movl -212(%rbp), %r11d
	cmpl $0, %r11d
	je .Lend_label_7
	movl $7, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.Lend_label_7:
	movsd .LC4(%rip), %xmm15
	movsd -200(%rbp), %xmm14
	comisd %xmm15, %xmm14
	movl $0, -216(%rbp)
	setne -216(%rbp)
	movl -216(%rbp), %r11d
	cmpl $0, %r11d
	je .Lend_label_8
	movl $8, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.Lend_label_8:
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .bss
   .align 4
   .globl zero
zero:
	.zero 4
   .section .rodata
   .align 16
.LC2:
	.quad 0
   .section .rodata
   .align 8
.LC1:
	.quad 9218868437227405312
   .section .rodata
   .align 16
.LC0:
	.quad 9223372036854775808
   .section .rodata
   .align 8
.LC3:
	.quad 4616189618054758400
   .section .rodata
   .align 8
.LC4:
	.quad 4617315517961601024
   .section .note.GNU-stack,"",@progbits
