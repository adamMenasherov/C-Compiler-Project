#include "TACKYConstructors.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TACKYInstruction* createLabelInstruction(char* label) {
    TACKYInstruction* inst = malloc(sizeof(TACKYInstruction));
    if (!inst) return NULL;
    inst->type = TACKY_LABEL;
    inst->instValue.label.label = label;
    return inst;
}

TACKYInstruction* createReturnInstruction(TACKYValue* retVal) {
    TACKYInstruction* inst = malloc(sizeof(TACKYInstruction));
    if (!inst) return NULL;
    inst->type = TACKY_RETURN;
    inst->instValue.returnVal.retVal = retVal;
    return inst;
}

TACKYInstruction* createUnaryInstruction(unaryType type, TACKYValue* src, TACKYValue* dest) {
    TACKYInstruction* inst = malloc(sizeof(TACKYInstruction));
    if (!inst) return NULL;
    inst->type = TACKY_UNARY;
    inst->instValue.unaryOp.type = type;
    inst->instValue.unaryOp.src = src;
    inst->instValue.unaryOp.dest = dest;
    return inst;
}

TACKYInstruction* createJumpInstruction(TACKYInstructionType jumpType, char* label, TACKYValue* condition) {
    TACKYInstruction* inst = malloc(sizeof(TACKYInstruction));
    if (!inst) return NULL;
    inst->type = jumpType;
    switch (jumpType) {
        case TACKY_JUMP:
            inst->instValue.jump.label = label;
            break;
        case TACKY_JUMP_IF_ZERO:
        case TACKY_JUMP_IF_NOT_ZERO:
            inst->instValue.condJump.label = label;
            inst->instValue.condJump.condition = condition;
            break;
        default:
            free(inst);
            return NULL;
    }

    return inst;
}

TACKYInstruction* createCopyInstruction(TACKYValue* src, TACKYValue* dest) {
    TACKYInstruction* inst = malloc(sizeof(TACKYInstruction));
    if (!inst) return NULL;
    inst->type = TACKY_COPY;
    inst->instValue.copy.src = src;
    inst->instValue.copy.dest = dest;
    return inst;
}

TACKYInstruction* createBinaryInstruction(binType type, TACKYValue* src1, TACKYValue* src2, TACKYValue* dest) {
    TACKYInstruction* inst = malloc(sizeof(TACKYInstruction));
    if (!inst) return NULL;
    inst->type = TACKY_BINARY;
    inst->instValue.binaryOp.binaryOpType = type;
    inst->instValue.binaryOp.src1 = src1;
    inst->instValue.binaryOp.src2 = src2;
    inst->instValue.binaryOp.dest = dest;
    return inst;
}

TACKYInstructionList* createTACKYInstructionList() {
    return InstructionArray_create();
}

void addInstructionToList(TACKYInstructionList* list, TACKYInstruction* instruction) {
    if (list) {
        InstructionArray_append(list, instruction);
    }
}

TACKYConstant* CreateTackyConstantNode(int val) {
    TACKYConstant* constantNode = malloc(sizeof(TACKYConstant));
    if (!constantNode) return NULL;

    constantNode->value = val;
    return constantNode;
}

TACKYValue* createTackyValueFromConstantNode(CConstant* const_node) {
    TACKYValue* tackyVal = calloc(1, sizeof(TACKYValue));
    if (!tackyVal) return NULL;

    tackyVal->type = TACKY_CONSTANT;
    tackyVal->constant = CreateTackyConstantNode(const_node->val);

    return tackyVal;
}

TACKYValue* createTackyValueFromConstant(int val) {
    TACKYValue* tackyVal = calloc(1, sizeof(TACKYValue));
    if (!tackyVal) return NULL;

    tackyVal->type = TACKY_CONSTANT;
    tackyVal->constant = CreateTackyConstantNode(val);

    return tackyVal;
}

TACKYValue* createTackyValueFromVar(CVar* var) {
    if (!var) return NULL;
    return createVarValue(var->identifier);
}

TACKYValue* createVarValue(char* identifier) {
    TACKYValue* tackyVal = malloc(sizeof(TACKYValue));
    if (!tackyVal) return NULL;

    tackyVal->type = TACKY_VAR;
    tackyVal->identifier = identifier;
    return tackyVal;
}

TACKYValue* copyTackyValue(TACKYValue* original) {
    if (!original) return NULL;

    TACKYValue* copy = malloc(sizeof(TACKYValue));
    if (!copy) return NULL;

    copy->type = original->type;

    if (original->type == TACKY_CONSTANT) {
        copy->constant = malloc(sizeof(TACKYConstant));
        if (!copy->constant) {
            free(copy);
            return NULL;
        }
        copy->constant->value = original->constant->value;
        copy->identifier = NULL;
    } else if (original->type == TACKY_VAR) {
        copy->identifier = strdup(original->identifier);
        copy->constant = NULL;
    }

    return copy;
}

TACKYInstruction* createFunCall(char* functionName, TACKYValueArray* args, TACKYValue* resultVar) {
    TACKYInstruction* inst = malloc(sizeof(TACKYInstruction));
    if (!inst) return NULL;

    inst->type = TACKY_FUNCTION_CALL;
    inst->instValue.funCall.functionName = strdup(functionName);
    if (!inst->instValue.funCall.functionName) {
        free(inst);
        return NULL;
    }

    inst->instValue.funCall.args = args;
    inst->instValue.funCall.resultVar = resultVar;
    return inst;
}

TACKYProgram* createTACKYProgram() {
    TACKYProgram* program = malloc(sizeof(TACKYProgram));
    if (!program) return NULL;

    program->functions = TACKYFunctionArray_create();
    if (!program->functions) {
        free(program);
        return NULL;
    }

    return program;
}

TACKYFunction* createTACKYFunction(char* function_name, IdentifierArray* parameters, TACKYInstructionList* instruction_list) {
    TACKYFunction* function = malloc(sizeof(TACKYFunction));
    if (!function) return NULL;

    function->function_name = strdup(function_name);
    if (!function->function_name) {
        free(function);
        return NULL;
    }

    function->parameters = IdentifierArray_copy(parameters);
    if (!function->parameters) {
        free(function->function_name);
        free(function);
        return NULL;
    }

    function->instruction_list = instruction_list;
    return function;
}