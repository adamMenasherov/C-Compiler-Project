#include "TACKYEmitters.h"
#include "TACKYConstructors.h"
#include "../../generateUtils.h"
#include <stdlib.h>
#include <stdio.h>

TACKYValue* emit_TACKY(CFactor* exp, TACKYInstructionList* instruction_list, int *isPostfixUnary) {
    if (!exp) return NULL;
    switch(exp->type) {
        case FACTOR_CONSTANT:
            return createTackyValueFromConstantNode(exp->exp.cnst);
        case FACTOR_VAR:
            return createTackyValueFromVar(exp->exp.var);
        case FACTOR_UNARY:
            return emit_TACKYUnary(exp, instruction_list, isPostfixUnary);
        case FACTOR_BINARY:
            return emit_TACKYBinary(exp, instruction_list);
        case FACTOR_ASSIGNMENT:
            return emit_TACKYAssignment(exp, instruction_list);
        case FACTOR_CONDITIONAL:
            return emit_TACKYConditional(exp, instruction_list);
        default:
            return NULL;
    }
}

TACKYValue* emit_TACKYUnary(CFactor* exp, TACKYInstructionList* instruction_list, int *isPostfixUnary) {
    if (isPostfixUnaryOp(exp->exp.unary->type)) {
        if (isPostfixUnary) *isPostfixUnary = 1;
        return createTackyValueFromVar(exp->exp.unary->exp->exp.var);
    }

    int isPostfix = 0;
    TACKYValue* src = emit_TACKY(exp->exp.unary->exp, instruction_list, &isPostfix);
    char* temp_name = generateTempName();
    TACKYValue* dst = createVarValue(temp_name);
    unaryType op = FromPostPreFixToRegular(exp->exp.unary->type);
    addInstructionToList(instruction_list,
        createUnaryInstruction(op, src, dst));
    if (isPostfix) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(exp->exp.unary->exp));

    return copyTackyValue(dst);
}

TACKYValue* emit_TACKYBinary(CFactor* exp, TACKYInstructionList* instruction_list) {
    if (exp->exp.binary->type == BIN_AND || exp->exp.binary->type == BIN_OR) {
        return shortCircuitTACKYInstruction(exp, instruction_list);
    }

    int isPostfixSrc1 = 0, isPostfixSrc2 = 0;
    TACKYValue* src1 = emit_TACKY(exp->exp.binary->left, instruction_list, &isPostfixSrc1);
    TACKYValue* src2 = emit_TACKY(exp->exp.binary->right, instruction_list, &isPostfixSrc2);
    char* temp_name = generateTempName();
    TACKYValue* dst = createVarValue(temp_name);
    binType op = exp->exp.binary->type;
    addInstructionToList(instruction_list,
        createBinaryInstruction(op, src1, src2, dst));

    if (isPostfixSrc1) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(exp->exp.binary->left));
    if (isPostfixSrc2) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(exp->exp.binary->right));

    return copyTackyValue(dst);
}

TACKYValue* emit_TACKYAssignment(CFactor* exp, TACKYInstructionList* instruction_list) {
    char* varName = exp->exp.assignment->exp1->exp.var->identifier;
    int isPostfixUnary = 0;
    TACKYValue* src = emit_TACKY(exp->exp.assignment->exp2, instruction_list, &isPostfixUnary);
    TACKYValue* dst = createVarValue(varName);
    addInstructionToList(instruction_list,
        createCopyInstruction(src, dst));

    if (isPostfixUnary) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(exp->exp.assignment->exp2));
    return copyTackyValue(dst);
}

TACKYValue* emit_TACKYConditional(CFactor* exp, TACKYInstructionList* instruction_list) {
    TACKYValue* cond = emit_TACKY(exp->exp.conditional->condition, instruction_list, NULL);
    char* e2Label = generateE2Label();
    char* endLabel = generateEndLabel();
    char* result = generateResultVarName();

    addInstructionToList(instruction_list,
        createJumpInstruction(TACKY_JUMP_IF_ZERO, e2Label, cond));

    TACKYValue* thenVal = emit_TACKY(exp->exp.conditional->then, instruction_list, NULL);
    addInstructionToList(instruction_list,
        createCopyInstruction(thenVal, createVarValue(result)));
    addInstructionToList(instruction_list,
        createJumpInstruction(TACKY_JUMP, endLabel, NULL));

    addInstructionToList(instruction_list,
        createLabelInstruction(e2Label));
    TACKYValue* elseVal = emit_TACKY(exp->exp.conditional->else_stmt, instruction_list, NULL);
    addInstructionToList(instruction_list,
        createCopyInstruction(elseVal, createVarValue(result)));

    addInstructionToList(instruction_list,
        createLabelInstruction(endLabel));

    return createVarValue(result);
}

TACKYInstruction* emitUnaryPostfixInstruction(CFactor* exp) {
    if (exp->type != FACTOR_UNARY || !isPostfixUnaryOp(exp->exp.unary->type)) {
        fprintf(stderr, "Error: Expected postfix unary expression\n");
        exit(1);
    }

    char* varName = exp->exp.unary->exp->exp.var->identifier;
    TACKYValue* src = createVarValue(varName);
    char* temp_name = generateTempName();
    TACKYValue* dst = createVarValue(temp_name);
    unaryType op = FromPostPreFixToRegular(exp->exp.unary->type);
    return createUnaryInstruction(op, src, dst);
}

TACKYValue* shortCircuitTACKYInstruction(CFactor* exp, TACKYInstructionList* instruction_list) {
    if (exp->type != FACTOR_BINARY) return NULL;

    binType op = exp->exp.binary->type;
    if (op != BIN_AND && op != BIN_OR) return NULL;

    int isPostfixSrc1 = 0, isPostfixSrc2 = 0;
    TACKYValue* leftVal = emit_TACKY(exp->exp.binary->left, instruction_list, &isPostfixSrc1);
    char* temp_name = generateTempName();
    char* endLabel = generateEndLabel();
    TACKYValue* resultVar = createVarValue(temp_name);
    TACKYInstructionType jumpType = (op == BIN_AND) ? TACKY_JUMP_IF_ZERO : TACKY_JUMP_IF_NOT_ZERO;
    int retVal = (op == BIN_AND) ? 1 : 0;
    char* shortCircuitLabel = (op == BIN_AND) ? generateFalseLabel() : generateTrueLabel();
    addInstructionToList(instruction_list,
        createJumpInstruction(jumpType, shortCircuitLabel, leftVal));

    TACKYValue* rightVal = emit_TACKY(exp->exp.binary->right, instruction_list, &isPostfixSrc2);
    addInstructionToList(instruction_list,
        createJumpInstruction(jumpType, shortCircuitLabel, rightVal));

    addInstructionToList(instruction_list,
        createCopyInstruction(createTackyValueFromConstant(retVal), resultVar));

    addInstructionToList(instruction_list,
        createJumpInstruction(TACKY_JUMP, endLabel, NULL));

    addInstructionToList(instruction_list,
        createLabelInstruction(shortCircuitLabel));

    addInstructionToList(instruction_list,
        createCopyInstruction(createTackyValueFromConstant(1 - retVal), resultVar));

    addInstructionToList(instruction_list,
        createLabelInstruction(endLabel));
    
    if (isPostfixSrc1) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(exp->exp.binary->left));
    if (isPostfixSrc2) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(exp->exp.binary->right));

    return copyTackyValue(resultVar);
}