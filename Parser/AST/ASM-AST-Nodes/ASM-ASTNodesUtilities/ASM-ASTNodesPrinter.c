#include "ASM-ASTNodesPrinter.h"
#include <stdio.h>


static const char* asmUnaryOperatorToString(ASMUnaryOperator op) {
    switch (op) {
        case ASM_UNARY_NEG: return "Neg";
        case ASM_UNARY_NOT: return "Not";
        default:  return "Unknown";
    }
}

static const char * asmBinaryOperatorToString(ASMBinaryOperator op) {
    switch (op) {
        case ASM_BINARY_ADD: return "Add";
        case ASM_BINARY_SUBTRACT: return "Sub";
        case ASM_BINARY_MULTIPLY: return "Mul";
        default: return "Unknown";
    }
}

static const char* getRegisterNameForASM_AST(Register reg) {
    switch (reg) {
        case AX: return "AX";
        case DX: return "DX";
        case R10: return "R10";
        case R11: return "R11";
        default: return "Unknown";
    }
}

static const char * getCondCodeString(ASMCondCode cond) {
    switch (cond) {
        case ASM_COND_CODE_E: return "E";
        case ASM_COND_CODE_NE: return "NE";
        case ASM_COND_CODE_L: return "L";
        case ASM_COND_CODE_LE: return "LE";
        case ASM_COND_CODE_G: return "G";
        case ASM_COND_CODE_GE: return "GE";
        default: return "Unknown";
    }
}


void printASMOperand(const ASMOperand* operand) {
    if (!operand) {
        printf("<null>");
        return;
    }

    switch (operand->type) {
        case ASM_OP_IMMEDIATE:
            printf("Imm(%d)", operand->OperandValue.immediate);
            break;

        case ASM_OP_REGISTER:
            printf("Reg(%s)", getRegisterNameForASM_AST(operand->OperandValue.reg));
            break;

        case ASM_OP_PSEUDO:
            if (operand->OperandValue.identifier) {
                printf("Pseudo(\"%s\")", operand->OperandValue.identifier);
            } else {
                printf("Pseudo(<null>)");
            }
            break;

        case ASM_OP_STACK:
            printf("Stack(%d)", operand->OperandValue.immediate);
            break;
        default: break;
    }
}

void printASMInstruction(const ASMInstruction* inst) {
    if (!inst) {
        printf("<null instruction>\n");
        return;
    }

    switch (inst->type) {
        case ASM_MOV:
            printf("Mov(");
            printASMOperand(inst->instValue.mov.operand1);
            printf(", ");
            printASMOperand(inst->instValue.mov.operand2);
            printf(")");
            break;

        case ASM_UNARY:
            printf("Unary(%s, ", asmUnaryOperatorToString(inst->instValue.unary.type));
            printASMOperand(inst->instValue.unary.op);
            printf(")");
            break;

        case ASM_ALLOCATESTACK:
            printf("AllocateStack(%d)", inst->instValue.allocatestack.size);
            break;

        case ASM_RET:
            printf("Ret");
            break;
        
        case ASM_BINARY:
            printf("Binary(%s, ", asmBinaryOperatorToString(inst->instValue.binary.type));
            printASMOperand(inst->instValue.binary.op1);
            printf(", ");
            printASMOperand(inst->instValue.binary.op2);
            printf(")");
            break;
        
        case ASM_IDIV:
            printf("IDiv(");
            printASMOperand(inst->instValue.idiv.divisor);
            printf(")");
            break;
        
        case ASM_CDQ:
            printf("CDQ");
            break;
        
        case ASM_LABEL:
            printf("Label(\"%s\")", inst->instValue.label.identifier);
            break;
        
        case ASM_CMP:
            printf("Cmp(");
            printASMOperand(inst->instValue.cmp.op1);
            printf(", ");
            printASMOperand(inst->instValue.cmp.op2);
            printf(")");
            break;
        
        case ASM_JUMP:
            printf("Jmp(\"%s\")", inst->instValue.jmp.label);
            break;
        
        case ASM_JUMPCC:
            printf("JumpCC(%s, \"%s\")", getCondCodeString(inst->instValue.jumpcc.cond), inst->instValue.jumpcc.label);
            break;
        case ASM_SETCC:
            printf("SetCC(%s, ", getCondCodeString(inst->instValue.setcc.cond));
            printASMOperand(inst->instValue.setcc.op);
            printf(")");
            break;

        default:
            printf("<unknown instruction type>");
            break;
    }
}

void printASMInstructionList(const ASMInstructionList* list) {
    if (!list) {
        printf("<null instruction list>\n");
        return;
    }

    ASMInstruction* current;
    for (current = list->head; current; current = current->next) {
        printf("        ");
        printASMInstruction(current);
        printf("\n");
    }
}

void printASMFunction(const ASMFunction* func) {
    if (!func) {
        printf("<null function>\n");
        return;
    }

    printf("Function(%s,\n", func->function_name ? func->function_name : "<unnamed>");
    printASMInstructionList(func->inst);
    printf(")\n");
}

void printASMProgram(const ASMProgram* program) {
    if (!program) {
        printf("<null program>\n");
        return;
    }

    printf("Program(\n");
    if (program->function_def) {
        printf("    ");
        printASMFunction(program->function_def);
    }
    printf(")\n");
}