#include "TACKY_AST_FREE.h"
#include <stdlib.h>

static void freeTackyFunctionCall(TACKYInstruction* instruction) {
    free(instruction->instValue.funCall.functionName);
    TACKYValueArray_freeWithValues(instruction->instValue.funCall.args);
    freeTackyValue(instruction->instValue.funCall.resultVar);
}

void freeTackyProgram(TACKYProgram* prog) {
    if (!prog) return;
    TACKYTopLevelArray_freeWithTopLevels(prog->topLevels);
    free(prog);
}

void freeTackyTopLevel(TACKYTopLevel* topLevel) {
    if (!topLevel) return;

    switch (topLevel->type) {
        case TACKY_STATIC_VAR:
            freeTackyStaticVar(topLevel->topLevel.staticVar);
            break;
        case TACKY_FUNC:
            freeTackyFunction(topLevel->topLevel.function);
            break;
        default:
            break;
    }

    free(topLevel);
}

void freeTackyStaticVar(TACKYStaticVar* staticVar) {
    if (!staticVar) return;
    free(staticVar);
}

void freeTackyFunction(TACKYFunction* func) {
    if (!func) return;
    free(func->function_name);
    IdentifierArray_free(func->parameters);
    freeTackyInstructionList(func->instruction_list);
    free(func);
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
        case TACKY_BINARY:
            freeTackyValue(instruction->instValue.binaryOp.src1);
            freeTackyValue(instruction->instValue.binaryOp.src2);
            freeTackyValue(instruction->instValue.binaryOp.dest);
            break;
        case TACKY_JUMP:
            free(instruction->instValue.jump.label);
            break;
        case TACKY_JUMP_IF_ZERO:
        case TACKY_JUMP_IF_NOT_ZERO:
            free(instruction->instValue.condJump.label);
            freeTackyValue(instruction->instValue.condJump.condition);
            break;
        case TACKY_LABEL:
            free(instruction->instValue.label.label);
            break;
        case TACKY_COPY:
            freeTackyValue(instruction->instValue.copy.src);
            freeTackyValue(instruction->instValue.copy.dest);
            break;
        case TACKY_RETURN:
            freeTackyValue(instruction->instValue.returnVal.retVal);
            break;
        case TACKY_FUNCTION_CALL:
            freeTackyFunctionCall(instruction);
            break;
        default:
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