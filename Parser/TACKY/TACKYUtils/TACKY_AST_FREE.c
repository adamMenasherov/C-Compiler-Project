#include "TACKY_AST_FREE.h"
#include <stdlib.h>

void freeTackyProgram(TACKYProgram* prog) {
    if (!prog) return;
    freeTackyFunction(prog->function_def);
    free(prog);
}

void freeTackyFunction(TACKYFunction* func) {
    if (!func) return;
    freeTackyInstructionList(func->instruction_list);
    freeTackyReturn(func->inst);
    free(func);
}

void freeTackyReturn(TACKYReturn* returnNode) {
    if (!returnNode) return;
    freeTackyValue(returnNode->val);
    free(returnNode);
}

void freeTackyInstructionList(TACKYInstructionList* list) {
    if (!list) return;
    for (int v = 0; v < InstructionArray_size(list); v++) {
        TACKYInstruction* inst = InstructionArray_get(list, v);
        if (inst) freeTackyInstruction(inst);
    }
    InstructionArray_free(list);
}

void freeTackyInstruction(TACKYInstruction* instruction) {
    if (!instruction) return;
    switch (instruction->type) {
        case TACKY_UNARY:
            freeTackyValue(instruction->instValue.unaryOp.src);
            freeTackyValue(instruction->instValue.unaryOp.dest);
            break;
    }
    free(instruction);
}

void freeTackyValue(TACKYValue* value) {
    if (!value) return;
    if (value->type == TACKY_CONSTANT)
        free(value->constant);
    else if (value->type == TACKY_VAR)
        free(value->identifier);
    free(value);
}

void freeTackyConstant(TACKYConstant* constant) {
    if (!constant) return;
    free(constant);
}