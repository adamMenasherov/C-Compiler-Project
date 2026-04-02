#include "../Parser/Parser.h"
#include <stdio.h>
#include "ASMGenerator.h"
#include <stdlib.h>
#include "ASM_AST_fix.h"
#include "../Parser/AST/ASM-AST-Nodes/ASM-ASTNodesUtilities/ASM-ASTNodesPrinter.h"


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
    fprintf(fp, "\tpushq %%rbp\n\tmovq %%rsp, %%rbp\n");
    ASMInstructionList* p = func->inst;
    ASMInstruction* current;
    for (current = p->head; current; current = current->next) {
        printInstructionsToASMFile(current, fp);
    }
}

void printInstructionsToASMFile(ASMInstruction* inst, FILE *fp) {
    switch(inst->type) {
        case ASM_MOV:
        {
            fputs("\tmovl ", fp);
            printOperandToASMFile(inst->instValue.mov.operand1, fp);
            fputs(", ", fp);
            printOperandToASMFile(inst->instValue.mov.operand2, fp);
            fputc('\n', fp);
            break;
        }
        case ASM_UNARY: {
            if (inst->instValue.unary.type == ASM_UNARY_NEG) {
                fputs("\tnegl ", fp);
                printOperandToASMFile(inst->instValue.unary.op, fp);
                fputc('\n', fp);
            }
            else if (inst->instValue.unary.type == ASM_UNARY_NOT) {
                fputs("\tnotl ", fp);
                printOperandToASMFile(inst->instValue.unary.op, fp);
                fputc('\n', fp);
            }
            break;
        }
        case ASM_ALLOCATESTACK: {
            fprintf(fp, "\tsubq $%d, %%rsp\n", inst->instValue.allocatestack.size);
            break;
        }
        case ASM_RET: {
            fputs("\tmovq %rbp, %rsp\n\tpopq %rbp\n", fp);
            fputs("\tret\n", fp);
            break;
        }
    }
}

void printOperandToASMFile(ASMOperand* op, FILE *fp) {
    if (!op) return;
    switch(op->type) {
        case ASM_OP_REGISTER: {
            fprintf(fp, "%%%s", getRegisterName(op->OperandValue.reg));
            break;
        }
            
        case ASM_OP_IMMEDIATE: {
            fprintf(fp, "$%d", op->OperandValue.immediate);
            break;
        }
            
        case ASM_OP_STACK: {
            fprintf(fp, "%d(%%rbp)", op->OperandValue.immediate);
            break;
        }
        default: break;
    }
}