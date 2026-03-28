#include "../Parser/Parser.h"
#include <stdio.h>
#include "ASMGenerator.h"


void generateASMFile(ASM_AST* ast, char* asm_file_name) {
    FILE* fp = fopen(asm_file_name, "w");
    if (!fp) {
        fprintf(stderr, "Couldn't create ASM file");
        exit(1);
    } 
    printAsmFileFromAst(ast, fp);
    printf("ASM file %s generated successfully.\n", asm_file_name);
    fclose(fp);
}

void printAsmFileFromAst(ASM_AST *ast, FILE *fp) {
    printProgramToASMFile(ast->prog, fp);
    fputs("   .section .note.GNU-stack,\"\",@progbits\n", fp);
}   

void printProgramToASMFile(ASMProgram* prog, FILE *fp) {
    printFunctionToASMFile(prog->function_def, fp);
}

void printFunctionToASMFile(ASMFunction* func, FILE *fp) {
    fprintf(fp, "   .globl %s\n%s:\n", func->function_name, func->function_name);
    Instruction* p = func->inst;
    while (p != NULL) {
        printInstructionsToASMFile(p, fp);
        p = p->next;
    }
}

void printInstructionsToASMFile(Instruction* inst, FILE *fp) {
    switch(inst->type) {
        case MOV:
            fputs("   movl   ", fp);
            printOperandToASMFile(inst->operand1, fp);
            fputs(", ", fp);
            printOperandToASMFile(inst->operand2, fp);
            fputc('\n', fp);
            break;
        case RET:
            fputs("   ret\n", fp);
            break;
    }
}

void printOperandToASMFile(Operand* op, FILE *fp) {
    if (!op) return;
    switch(op->type) {
        case REGISTER:
            fputs("%eax", fp);
            break;
        case IMMEDIATE:
            fprintf(fp, "$%d", op->OperandValue);
            break;
    }
}