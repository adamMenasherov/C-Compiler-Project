#include "TACKYConstructors.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "TACKYProgram_FREE.h"
#include "../../Common/SharedTypeRank.h"

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

TACKYInstruction* createDoubleIntCastInstruction(TACKYValue* src, TACKYValue* dest, TACKYInstructionType instType) {
    TACKYInstruction* inst = malloc(sizeof(TACKYInstruction));
    if (!inst) return NULL;
    inst->type = instType;
    inst->instValue.doubleIntCast.src = src;
    inst->instValue.doubleIntCast.dest = dest;
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

TACKYInstruction* createSignExtendInstruction(TACKYValue* src, TACKYValue* dest) {
    TACKYInstruction* inst = malloc(sizeof(TACKYInstruction));
    if (!inst) return NULL;
    inst->type = TACKY_SIGN_EXTEND;
    inst->instValue.signExtend.src = src;
    inst->instValue.signExtend.dest = dest;
    return inst;
}
TACKYInstruction* createTruncateInstruction(TACKYValue* src, TACKYValue* dest) {
    TACKYInstruction* inst = malloc(sizeof(TACKYInstruction));
    if (!inst) return NULL;
    inst->type = TACKY_TRUNCATE;
    inst->instValue.truncate.src = src;
    inst->instValue.truncate.dest = dest;
    return inst;
}

TACKYInstruction* createZeroExtendInstruction(TACKYValue* src, TACKYValue* dest) {
    TACKYInstruction* inst = malloc(sizeof(TACKYInstruction));
    if (!inst) return NULL;
    inst->type = TACKY_ZERO_EXTEND;
    inst->instValue.zeroExtend.src = src;
    inst->instValue.zeroExtend.dest = dest;
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

TACKYConstant* CreateTackyConstantNode(uint64_t intValue, double doubleValue, constantType type) {
    TACKYConstant* constantNode = malloc(sizeof(TACKYConstant));
    if (!constantNode) return NULL;

    if (type == CONST_FLOATING_POINT) {
        constantNode->val.doubleValue = doubleValue;
    } else {
        constantNode->val.intValue = intValue;
    }
    constantNode->type = type;
    return constantNode;
}

TACKYValue* createTackyValueFromConstantNode(CConstant* const_node) {
    TACKYValue* tackyVal = calloc(1, sizeof(TACKYValue));
    if (!tackyVal) return NULL;

    tackyVal->type = TACKY_CONSTANT;
    tackyVal->constant = CreateTackyConstantNode(const_node->value.intValue, const_node->value.doubleValue, const_node->type);

    return tackyVal;
}

TACKYInstruction* generateTACKYDoubleIntCast(TACKYValue* src, TACKYValue* dst, CType* srcType, CType* dstType, SymbolTable* symTable) {
    if (!dst) return NULL;
    (void)symTable;
    int srcIsDouble = srcType && srcType->kind == CTYPE_DOUBLE;
    int dstIsDouble = dstType && dstType->kind == CTYPE_DOUBLE;
    int srcIsSigned = srcType && (srcType->kind == CTYPE_INT || srcType->kind == CTYPE_LONG);
    int dstIsSigned = dstType && (dstType->kind == CTYPE_INT || dstType->kind == CTYPE_LONG);

    if (srcIsDouble && !dstIsDouble) {
        return createDoubleIntCastInstruction(src, dst, dstIsSigned ? TACKY_DOUBLE_TO_INT : TACKY_DOUBLE_TO_UINT);
    } else if (!srcIsDouble && dstIsDouble) {
        return createDoubleIntCastInstruction(src, dst, srcIsSigned ? TACKY_INT_TO_DOUBLE : TACKY_UINT_TO_DOUBLE);
    }
    return createCopyInstruction(src, dst);
}

TACKYValue* createTackyValueFromConstant(uint64_t intValue, double doubleValue, constantType type) {
    TACKYValue* tackyVal = calloc(1, sizeof(TACKYValue));
    if (!tackyVal) return NULL;

    tackyVal->type = TACKY_CONSTANT;
    tackyVal->constant = CreateTackyConstantNode(intValue, doubleValue, type);

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
    tackyVal->identifier = strdup(identifier);
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
        copy->constant->val = original->constant->val;
        copy->constant->type = original->constant->type;
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

    program->topLevels = TACKYTopLevelArray_create();
    if (!program->topLevels) {
        free(program);
        return NULL;
    }

    return program;
}

TACKYFunction* createTACKYFunction(char* function_name, IdentifierArray* parameters, TACKYInstructionList* instruction_list, int global) {
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
    function->global = global;
    return function;
}

TACKYTopLevel* createTACKYTopLevelFromFunction(TACKYFunction* tackyFunc, SymbolTable* symTable) {
    if (!tackyFunc) return NULL;

    TACKYTopLevel* topLevel = malloc(sizeof(TACKYTopLevel));
    if (!topLevel) {
        freeTackyFunction(tackyFunc);
        return NULL;
    }

    topLevel->type = TACKY_FUNC;
    topLevel->topLevel.function = tackyFunc;
    return topLevel;
}

TACKYTopLevel* createTACKYTopLevelFromStaticVar(TACKYStaticVar* staticVar) {
    if (!staticVar) return NULL;

    TACKYTopLevel* topLevel = malloc(sizeof(TACKYTopLevel));
    if (!topLevel) {
        freeTackyStaticVar(staticVar);
        return NULL;
    }

    topLevel->type = TACKY_STATIC_VAR;
    topLevel->topLevel.staticVar = staticVar;
    return topLevel;
}

TACKYStaticVar* createTACKYStaticVar(char* identifier, int global, uint64_t intVal, double doubleVal, initialValueStaticInitType staticInitType) {
    TACKYStaticVar* staticVar = malloc(sizeof(TACKYStaticVar));
    if (!staticVar) return NULL;

    staticVar->identifier = strdup(identifier);
    if (!staticVar->identifier) {
        free(staticVar);
        return NULL;
    }

    staticVar->global = global;
    staticVar->type = initialStaticTypeToTACKYStaticVarType(staticInitType);
    if (staticVar->type == TACKY_DOUBLE) {
        staticVar->initVal.val.doubleVal = doubleVal;
    } else {
        staticVar->initVal.val.intVal = intVal;
    }
    staticVar->initVal.staticInitType = staticInitType;
    return staticVar;
}