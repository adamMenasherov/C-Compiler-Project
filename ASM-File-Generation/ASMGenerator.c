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
    printFunctionPrologueToASMFile(func, fp);
    ASMInstructionList* p = func->inst;
    ASMInstruction* current;
    for (current = p->head; current; current = current->next) {
        printInstructionsToASMFile(current, fp);
    }
}

void printFunctionPrologueToASMFile(ASMFunction* func, FILE *fp) {
    fprintf(fp, "   .globl %s\n%s:\n", func->function_name, func->function_name);
    fprintf(fp, "\tpushq %%rbp\n\tmovq %%rsp, %%rbp\n");
}

void printFunctionEpilogueToASMFile(FILE* fp) {
    fputs("\tmovq %rbp, %rsp\n\tpopq %rbp\n", fp);
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
            fprintf(fp, "\t%s ", asmUnaryOperatorToString(inst->instValue.unary.type));
            printOperandToASMFile(inst->instValue.unary.op, fp);
            fputc('\n', fp);
            break;
        }

        case ASM_BINARY: {
            fprintf(fp, "\t%s ", asmBinaryOperatorToString(inst->instValue.binary.type));
            printOperandToASMFile(inst->instValue.binary.op1, fp);
            fputs(", ", fp);
            printOperandToASMFile(inst->instValue.binary.op2, fp);
            fputc('\n', fp);
            break;
        }
    
        case ASM_IDIV: {
            fputs("\tidivl ", fp);
            printOperandToASMFile(inst->instValue.idiv.divisor, fp);
            fputc('\n', fp);
            break;
        }
        case ASM_ALLOCATESTACK: {
            fprintf(fp, "\tsubq $%d, %%rsp\n", inst->instValue.allocatestack.size);
            break;
        }
        case ASM_RET: {
            printFunctionEpilogueToASMFile(fp);
            fputs("\tret\n", fp);
            break;
        }
        
        case ASM_CDQ: {
            fputs("\tcdq\n", fp);
            break;
        }
    }
}

void printOperandToASMFile(ASMOperand* op, FILE *fp) {
    if (!op) return;
    switch(op->type) {
        case ASM_OP_REGISTER: {
            fprintf(fp, "%%%s", getRegisterNameForCodeEmission(op->OperandValue.reg));
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

const char* getRegisterNameForCodeEmission(Register reg) {
    switch (reg) {
        case AX: return "eax";
        case DX: return "edx";
        case R10: return "r10d";
        case R11: return "r11d";
        default: return "<unknown register>";
    }
}

const char * asmUnaryOperatorToString(ASMUnaryOperator op) {
    switch (op) {
        case ASM_UNARY_NEG: return "negl";
        case ASM_UNARY_NOT: return "notl";
        default: return "<unknown unary operator>";
    }
}
const char * asmBinaryOperatorToString(ASMBinaryOperator op) {
    switch (op) {
        case ASM_BINARY_ADD: return "addl";
        case ASM_BINARY_SUBTRACT: return "subl";
        case ASM_BINARY_MULTIPLY: return "imull";
        default: return "<unknown binary operator>";
    }
}