   .globl main
   .text
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl foo(%rip), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

   .data
foo:
	.long 4
   .section .note.GNU-stack,"",@progbits
