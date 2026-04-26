.section .text
.global main
main:
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $16, %rsp          # Reserve space on stack (16-byte aligned)

    movl    $4, -4(%rbp)       # x = 4
    cmpl    $5, -4(%rbp)       # if (x > 5)
    jle     .else1             # jump if 4 <= 5
    
    movl    $1, %eax           # result = 1
    jmp     .return

.else1:
    shll    $1, -4(%rbp)                          
    movl -4(%rbp), %eax

.return:
    leave 
    ret
