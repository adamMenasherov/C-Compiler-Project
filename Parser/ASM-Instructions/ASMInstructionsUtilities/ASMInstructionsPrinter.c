#include "ASMInstructionsPrinter.h"
#include "Parser/Common/SharedTypeNames.h"
#include <stdio.h>


static const char* asmUnaryOperatorToString(ASMUnaryOperator op) {
    switch (op) {
        case ASM_UNARY_NEG: return "Neg";
        case ASM_UNARY_NOT: return "Not";
        case ASM_UNARY_DEC: return "Dec";
        case ASM_UNARY_INC: return "Inc";
        default:  return "Unknown";
    }
}

static const char * asmBinaryOperatorToString(ASMBinaryOperator op) {
    switch (op) {
        case ASM_BINARY_ADD: return "Add";
        case ASM_BINARY_SUBTRACT: return "Sub";
        case ASM_BINARY_MULTIPLY: return "Mul";
        case ASM_BINARY_AND: return "And";
        case ASM_BINARY_OR: return "Or";
        case ASM_BINARY_XOR: return "Xor";
        case ASM_BINARY_SHIFT_LEFT: return "Sal";
        case ASM_BINARY_SHIFT_RIGHT: return "Sar";
        default: return "Unknown";
    }
}

static const char* getRegisterNameForASM_AST(Register reg) {
    switch (reg) {
        case AX: return "AX";
        case CX: return "CX";
        case DX: return "DX";
        case DI: return "DI";
        case SI: return "SI";
        case SP: return "SP";
        case R8: return "R8";
        case R9: return "R9";
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
        case ASM_COND_CODE_A: return "A";
        case ASM_COND_CODE_AE: return "AE";
        case ASM_COND_CODE_B: return "B";
        case ASM_COND_CODE_BE: return "BE";
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
                printf("Imm(%ld)", (long)operand->OperandValue.immediate);
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
                printf("Stack(%ld)", (long)operand->OperandValue.immediate);
            break;
        
        case ASM_OP_DATA:
            if (operand->OperandValue.identifier) {
                printf("Data(\"%s\")", operand->OperandValue.identifier);
            } else {
                printf("Data(<null>)");
            }
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
            printASMType(inst->instValue.mov.asmType);
            printf(", ");
            printASMOperand(inst->instValue.mov.operand1);
            printf(", ");
            printASMOperand(inst->instValue.mov.operand2);
            printf(")");
            break;

        case ASM_MOVSX:
            printf("Movsx(");
            printASMOperand(inst->instValue.movsx.operand1);
            printf(", ");
            printASMOperand(inst->instValue.movsx.operand2);
            printf(")");
            break;
        
        case ASM_MOVZEROEXTEND:
            printf("MovZeroExtend(");
            printASMOperand(inst->instValue.movZeroExtend.operand1);
            printf(", ");
            printASMOperand(inst->instValue.movZeroExtend.operand2);
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

        case ASM_DEALLOCATESTACK:
            printf("DeallocateStack(%d)", inst->instValue.deallocatestack.size);
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

        case ASM_DIV:
            printf("Div(");
            printASMType(inst->instValue.div.asmType);
            printf(", ");
            printASMOperand(inst->instValue.div.divisor);
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

        case ASM_PUSH:
            printf("Push(");
            printASMOperand(inst->instValue.push.op);
            printf(")");
            break;

        case ASM_POP:
            printf("Pop(");
            printASMOperand(inst->instValue.pop.op);
            printf(")");
            break;

        case ASM_CALL:
            printf("Call(\"%s\")", inst->instValue.call.functionName);
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

void printASMType(ASMType type) {
    switch (type) {
        case ASM_LONGWORD:
            printf("LongWord");
            break;
        case ASM_QUADWORD:
            printf("QuadWord");
            break;
        default:
            printf("UnknownType");
            break;
    }
}

void printASMFunction(const ASMFunction* func) {
    if (!func) {
        printf("<null function>\n");
        return;
    }

    printf("Function(%s,\n, global=%d", func->function_name ? func->function_name : "<unnamed>", func->global);
    printASMInstructionList(func->inst);
    printf(")\n");
}

static void printASMStaticVar(const ASMStaticVar* staticVar) {
    if (!staticVar) {
        printf("StaticVar(<null>)\n");
        return;
    }

        printf("StaticVar(%s, global=%d, initVal=%ld, initType=%s)\n",
           staticVar->identifier ? staticVar->identifier : "<unnamed>",
           staticVar->global,
           staticVar->initVal.val,
            getStaticInitTypeName(staticVar->initVal.staticInitType));
}

static void printASMTopLevel(const ASMTopLevel* topLevel) {
    if (!topLevel) {
        printf("<null top-level>\n");
        return;
    }

    if (topLevel->type == ASM_TOP_LEVEL_FUNCTION) {
        printASMFunction(topLevel->topLevel.function);
    } else if (topLevel->type == ASM_TOP_LEVEL_STATIC_VAR) {
        printASMStaticVar(topLevel->topLevel.staticVar);
    }
}

void printASMProgram(const ASMProgram* program) {
    if (!program) {
        printf("<null program>\n");
        return;
    }

    printf("Program(\n");
    for (int i = 0; i < ASMTopLevelArray_size(program->topLevels); i++) {
        ASMTopLevel* topLevel = ASMTopLevelArray_get(program->topLevels, i);
        if (!topLevel) continue;
        printf("    ");
        printASMTopLevel(topLevel);
    }
    printf(")\n");
}