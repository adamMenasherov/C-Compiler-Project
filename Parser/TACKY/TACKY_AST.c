#include "TACKY_AST.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TACKYFunction* parseTACKYFunction(CFunction* func) {
    TACKYInstructionList* instruction_list = createTACKYInstructionList();
    TACKYReturn* returnNode = parseTACKYReturn(func->body, instruction_list);
    TACKYFunction* tackyFunc = malloc(sizeof(TACKYFunction));
    if (!tackyFunc) return NULL;

    tackyFunc->function_name = func->function_name;
    tackyFunc->inst = returnNode;
    tackyFunc->instruction_list = instruction_list;
    return tackyFunc;
}

TACKYProgram* parseTACKYProgram(CProgram* program) {
    TACKYFunction* function_def = parseTACKYFunction(program->function_def);
    TACKYProgram* tackyProg = malloc(sizeof(TACKYProgram));
    if (!tackyProg) return NULL;

    tackyProg->function_def = function_def;
    return tackyProg;
}

TACKYReturn* parseTACKYReturn(CReturn* returnNode, TACKYInstructionList* instructionList) {
    TACKYReturn* tackyRet = malloc(sizeof(TACKYReturn));
    if (!tackyRet) return NULL;

    TACKYValue* ret_val = emit_TACKY(returnNode->exp, instructionList);
    tackyRet->val = ret_val;
    return tackyRet;
}

TACKYValue* emit_TACKY(CExpression* exp, TACKYInstructionList* instruction_list) {
    switch(exp->type) {
        case EXP_CONSTANT: {
            return createTackyValueFromConstantNode(exp->exp.cnst);
            break;
        }
        case EXP_UNARY: {
            TACKYValue* src = emit_TACKY(exp->exp.unary->exp,instruction_list);
            char* temp_name = generateTempName();
            TACKYValue* dst = createVarValue(temp_name);
            unaryType op = exp->exp.unary->type;
            addInstructionToList(instruction_list, 
                    createUnaryInstruction(op, src, dst));
            return dst;
        }
        default: return NULL;
    }
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


TACKYInstructionList* createTACKYInstructionList() {
    TACKYInstructionList* tackyList = malloc(sizeof(TACKYInstructionList));
    if (!tackyList) return NULL;
    tackyList->instructions = malloc(sizeof(TACKYInstruction*) * 16);
    if (!tackyList->instructions) return NULL;
    tackyList->arrSize = 16;
    tackyList->currSize = 0;
    tackyList->cursor = 0;
    return tackyList;
}

void addInstructionToList(TACKYInstructionList* list, TACKYInstruction* instruction) {
    if (list->currSize >= list->arrSize) {
        list->instructions = realloc(list->instructions, 
            list->arrSize * 2 * sizeof(TACKYInstruction*));
        if (!list->instructions) return;
        list->arrSize *= 2;
    }
    list->instructions[list->currSize++] = instruction;
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

TACKYValue* createVarValue(char* identifier) {
    TACKYValue* tackyVal = malloc(sizeof(TACKYValue));
    if (!tackyVal) return NULL;

    tackyVal->type = TACKY_VAR;
    tackyVal->identifier = identifier;
    return tackyVal;
}

char* generateTempName() {
    static int curr = 0;
    char* temp_name = malloc(strlen("tmp.") + 10);
    if (!temp_name) return NULL;
    sprintf(temp_name, "tmp.%d", curr++);
    return temp_name;
}