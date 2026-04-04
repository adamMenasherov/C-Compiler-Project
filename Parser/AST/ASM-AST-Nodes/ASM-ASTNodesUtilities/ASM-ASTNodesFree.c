#include "ASM-ASTNodesFree.h"
#include "../../../../DataStructures/HashTable/HashTable.h"
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
    freeHashTable(func->pseudoTable);
    free(func);
}


void freeASMProgram(ASMProgram* program) {
    if (!program) return;
    ASMFunction* current = program->function_def;
    freeASMFunction(current);
    free(program);
}