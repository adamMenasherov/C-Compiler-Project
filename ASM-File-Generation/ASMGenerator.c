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
            printOperandToASMFile(inst->instValue.mov.operand1, fp, REGISTER_32_BIT);
            fputs(", ", fp);
            printOperandToASMFile(inst->instValue.mov.operand2, fp, REGISTER_32_BIT);
            fputc('\n', fp);
            break;
        }
        case ASM_UNARY: {
            fprintf(fp, "\t%s ", asmUnaryOperatorToString(inst->instValue.unary.type));
            printOperandToASMFile(inst->instValue.unary.op, fp, REGISTER_32_BIT);
            fputc('\n', fp);
            break;
        }
        case ASM_JUMP: {
            fprintf(fp, "\tjmp .L%s\n", inst->instValue.jmp.label);
            break;
        }
        case ASM_LABEL: {
            fprintf(fp, ".L%s:\n", inst->instValue.label.identifier);
            break;
        }
        case ASM_SETCC: {
            fprintf(fp, "\tset%s ", asmCondCodeToString(inst->instValue.setcc.cond));
            printOperandToASMFile(inst->instValue.setcc.op, fp, REGISTER_8_BIT);
            fputc('\n', fp);
            break;
        }
        case ASM_JUMPCC: {
            fprintf(fp, "\tj%s .L%s\n", 
                        asmCondCodeToString(inst->instValue.jumpcc.cond), inst->instValue.jumpcc.label);
            break;
        }

        case ASM_CMP: {
            fputs("\tcmpl ", fp);
            printOperandToASMFile(inst->instValue.cmp.op1, fp, REGISTER_32_BIT);
            fputs(", ", fp);
            printOperandToASMFile(inst->instValue.cmp.op2, fp, REGISTER_32_BIT);
            fputc('\n', fp);
            break;
        }

        case ASM_BINARY: {
            fprintf(fp, "\t%s ", asmBinaryOperatorToString(inst->instValue.binary.type));
            if (inst->instValue.binary.type == ASM_BINARY_SHIFT_LEFT || inst->instValue.binary.type == ASM_BINARY_SHIFT_RIGHT) 
                printOperandToASMFile(inst->instValue.binary.op1, fp, REGISTER_8_BIT);
            else 
                printOperandToASMFile(inst->instValue.binary.op1, fp, REGISTER_32_BIT);
            fputs(", ", fp);
            printOperandToASMFile(inst->instValue.binary.op2, fp, REGISTER_32_BIT);
            fputc('\n', fp);
            break;
        }
    
        case ASM_IDIV: {
            fputs("\tidivl ", fp);
            printOperandToASMFile(inst->instValue.idiv.divisor, fp, REGISTER_32_BIT);
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

        case ASM_PUSH: {
            fputs("\tpushq ", fp);
            printOperandToASMFile(inst->instValue.push.op, fp, REGISTER_64_BIT);
            fputc('\n', fp);
            break;
        }

        case ASM_POP: {
            fputs("\tpopq ", fp);
            printOperandToASMFile(inst->instValue.pop.op, fp, REGISTER_64_BIT);
            fputc('\n', fp);
            break;
        }
        
        case ASM_CDQ: {
            fputs("\tcdq\n", fp);
            break;
        }
    }
}

void printOperandToASMFile(ASMOperand* op, FILE *fp, REGISTER_SIZE size) 
{
    if (!op) return;
    switch(op->type) {
        case ASM_OP_REGISTER: {
            fprintf(fp, "%%%s", getRegisterNameForCodeEmission(op->OperandValue.reg, size));
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

const char* getRegisterNameForCodeEmission(Register reg, REGISTER_SIZE size) {
    switch (size) {
        case REGISTER_8_BIT:
            switch (reg) {
                case AX: return "al";
                case DX: return "dl";
                case CX: return "cl";
                case R10: return "r10b";
                case R11: return "r11b";
                default: return "<unknown register>";
            }
        case REGISTER_16_BIT:
            switch (reg) {
                case AX: return "ax";
                case DX: return "dx";
                case CX: return "cx";
                case R10: return "r10w";
                case R11: return "r11w";
                default: return "<unknown register>";
            }
        case REGISTER_32_BIT:
            switch (reg) {
                case AX: return "eax";
                case DX: return "edx";
                case CX: return "ecx";
                case R10: return "r10d";
                case R11: return "r11d";
                default: return "<unknown register>";
            }
        case REGISTER_64_BIT:
            switch (reg) {
                case AX: return "rax";
                case DX: return "rdx";
                case CX: return "rcx";
                case R10: return "r10";
                case R11: return "r11";
                default: return "<unknown register>";
            }
        default:
            return "<unknown register size>";
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
        case ASM_BINARY_AND: return "andl";
        case ASM_BINARY_OR: return "orl";
        case ASM_BINARY_XOR: return "xorl";
        case ASM_BINARY_SHIFT_LEFT: return "sall";
        case ASM_BINARY_SHIFT_RIGHT: return "sarl";
        default: return "<unknown binary operator>";
    }
}

const char* asmCondCodeToString(ASMCondCode cond) {
    switch (cond) {
        case ASM_COND_CODE_E: return "e";
        case ASM_COND_CODE_NE: return "ne";
        case ASM_COND_CODE_L: return "l";
        case ASM_COND_CODE_LE: return "le";
        case ASM_COND_CODE_G: return "g";
        case ASM_COND_CODE_GE: return "ge";
        default: return "<unknown condition code>";
    }
}