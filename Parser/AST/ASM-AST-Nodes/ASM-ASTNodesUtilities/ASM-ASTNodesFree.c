#include "ASM-ASTNodesFree.h"
#include "../../../../DataStructures/Map/Wrappers/CharIntMap.h"
#include <stdlib.h>

void freeASMOperand(ASMOperand* operand) {
    if (!operand) return;
    if (operand->type == ASM_OP_PSEUDO) {
        free(operand->OperandValue.identifier);
    }
    free(operand);
}


void freeASMInstruction(ASMInstruction* inst) {
    if (!inst) return;
    switch (inst->type) {
        case ASM_MOV:
            freeASMOperand(inst->instValue.mov.operand1);
            freeASMOperand(inst->instValue.mov.operand2);
            break;
        case ASM_UNARY:
            freeASMOperand(inst->instValue.unary.op);
            break;
        case ASM_BINARY:
            freeASMOperand(inst->instValue.binary.op1);
            freeASMOperand(inst->instValue.binary.op2);
            break;
        case ASM_IDIV:
            freeASMOperand(inst->instValue.idiv.divisor);
            break;
        case ASM_LABEL:
            free(inst->instValue.label.identifier);
            break;
        case ASM_CMP:
            freeASMOperand(inst->instValue.cmp.op1);
            freeASMOperand(inst->instValue.cmp.op2);
            break;
        case ASM_JUMP:
            free(inst->instValue.jmp.label);
            break;
        case ASM_JUMPCC:
            free(inst->instValue.jumpcc.label);
            break;
        case ASM_SETCC:
            freeASMOperand(inst->instValue.setcc.op);
            break;
        case ASM_PUSH:
            freeASMOperand(inst->instValue.push.op);
            break;
        case ASM_POP:
            freeASMOperand(inst->instValue.pop.op);
            break;
        case ASM_CALL:
            free(inst->instValue.call.functionName);
            break;
        default: break;
    }
    free(inst);
}

/**
 * Frees the memory allocated for an ASM instruction list and all its instructions.
 * @param list The ASM instruction list to free
 */
void freeASMInstructionList(ASMInstructionList* list) {
    if (!list) return;
    ASMInstruction* current = list->head;
    while (current) {
        ASMInstruction* next = current->next;
        freeASMInstruction(current);
        current = next;
    }
    free(list);
}


void freeASMFunction(ASMFunction* func) {
    if (!func) return;
    free(func->function_name);
    freeASMInstructionList(func->inst);
    freeCharIntMap(func->pseudoTable);
    free(func);
}

void freeASMTopLevel(ASMTopLevel* topLevel) {
    if (!topLevel) return;

    if (topLevel->type == ASM_TOP_LEVEL_FUNCTION) {
        freeASMFunction(topLevel->topLevel.function);
    } else if (topLevel->type == ASM_TOP_LEVEL_STATIC_VAR) {
        ASMStaticVar* staticVar = topLevel->topLevel.staticVar;
        if (staticVar) {
            free(staticVar->identifier);
            free(staticVar);
        }
    }

    free(topLevel);
}


void freeASMProgram(ASMProgram* program) {
    if (!program) return;
    ASMTopLevelArray_freeWithTopLevels(program->topLevels);
    free(program);
}