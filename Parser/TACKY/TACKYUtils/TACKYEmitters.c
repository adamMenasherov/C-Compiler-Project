#include "TACKYEmitters.h"
#include "TACKYConstructors.h"
#include "../../generateUtils.h"
#include "../../../Semantic/utils/SemanticUtils/SemanticUtils.h"
#include "../../AST/C-AST-Nodes/C-ASTNodeUtilities/TokenExpect/C-ASTNodeExpect.h"
#include <stdlib.h>
#include <stdio.h>

static specifierType getCastSourceType(CFactor* sourceExpr, SymbolTable* symTable) {
    if (!sourceExpr) return SPEC_NULL;

    switch (sourceExpr->type) {
        case FACTOR_VAR: {
            IdentifierTypeInfo* info = symbolTableLookup(symTable, sourceExpr->exp.var->identifier);
            if (!info) return sourceExpr->valueType;
            return identifierTypeToSpecifierType(info->type);
        }
        case FACTOR_CONSTANT:
            return constantTypeToSpecifierType(sourceExpr->exp.cnst->type);
        case FACTOR_CAST:
            return sourceExpr->exp.cast->targetType;
        default:
            return sourceExpr->valueType;
    }
}

TACKYValue* emit_TACKY(CFactor* exp, TACKYInstructionList* instruction_list, int *isPostfixUnary, SymbolTable* symTable) {
    if (!exp) return NULL;
    switch(exp->type) {
        case FACTOR_CONSTANT:
            return createTackyValueFromConstantNode(exp->exp.cnst);
        case FACTOR_VAR:
            return createTackyValueFromVar(exp->exp.var);
        case FACTOR_UNARY:
            return emit_TACKYUnary(exp, instruction_list, isPostfixUnary, symTable);
        case FACTOR_BINARY:
            return emit_TACKYBinary(exp, instruction_list, symTable);
        case FACTOR_ASSIGNMENT:
            return emit_TACKYAssignment(exp, instruction_list, symTable);
        case FACTOR_CONDITIONAL:
            return emit_TACKYConditional(exp, instruction_list, symTable);
        case FACTOR_FUNCTION_CALL:
             return emit_TACKYFunctionCall(exp, instruction_list, symTable);
        case FACTOR_CAST:
             return emit_TACKYCast(exp, instruction_list, symTable);
        default:
            return NULL;
    }
}

TACKYValue* emit_TACKYUnary(CFactor* exp, TACKYInstructionList* instruction_list, int *isPostfixUnary, SymbolTable* symTable) {
    if (isPostfixUnaryOp(exp->exp.unary->type)) {
        if (isPostfixUnary) *isPostfixUnary = 1;
        return createTackyValueFromVar(exp->exp.unary->exp->exp.var);
    }

    int isPostfix = 0;
    TACKYValue* src = emit_TACKY(exp->exp.unary->exp, instruction_list, &isPostfix, symTable);
    TACKYValue* dst = makeTACKYVariable(getType(exp), symTable);
    unaryType op = FromPostPreFixToRegular(exp->exp.unary->type);
    addInstructionToList(instruction_list,
        createUnaryInstruction(op, src, dst));
    if (isPostfix) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(exp->exp.unary->exp, symTable));

    return copyTackyValue(dst);
}

TACKYValue* emit_TACKYBinary(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    if (exp->exp.binary->type == BIN_AND || exp->exp.binary->type == BIN_OR) {
        return shortCircuitTACKYInstruction(exp, instruction_list, symTable);
    }

    int isPostfixSrc1 = 0, isPostfixSrc2 = 0;
    TACKYValue* src1 = emit_TACKY(exp->exp.binary->left, instruction_list, &isPostfixSrc1, symTable);
    TACKYValue* src2 = emit_TACKY(exp->exp.binary->right, instruction_list, &isPostfixSrc2, symTable);
    TACKYValue* dst = makeTACKYVariable(getType(exp), symTable);
    binType op = exp->exp.binary->type;
    addInstructionToList(instruction_list,
        createBinaryInstruction(op, src1, src2, dst));

    if (isPostfixSrc1) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(exp->exp.binary->left, symTable));
    if (isPostfixSrc2) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(exp->exp.binary->right, symTable));

    return copyTackyValue(dst);
}

TACKYValue* emit_TACKYAssignment(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    char* varName = exp->exp.assignment->exp1->exp.var->identifier;
    int isPostfixUnary = 0;
    TACKYValue* src = emit_TACKY(exp->exp.assignment->exp2, instruction_list, &isPostfixUnary, symTable);
    TACKYValue* dst = createVarValue(varName);
    addInstructionToList(instruction_list,
        createCopyInstruction(src, dst));

    if (isPostfixUnary) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(exp->exp.assignment->exp2, symTable));
    return copyTackyValue(dst);
}

TACKYValue* emit_TACKYConditional(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    TACKYValue* cond = emit_TACKY(exp->exp.conditional->condition, instruction_list, NULL, symTable);
    char* e2Label = generateE2Label();
    char* endLabel = generateEndLabel();
    TACKYValue* resultVar = makeTACKYVariable(getType(exp), symTable);

    addInstructionToList(instruction_list,
        createJumpInstruction(TACKY_JUMP_IF_ZERO, e2Label, cond));

    TACKYValue* thenVal = emit_TACKY(exp->exp.conditional->then, instruction_list, NULL, symTable);
    addInstructionToList(instruction_list,
        createCopyInstruction(thenVal, copyTackyValue(resultVar)));
    addInstructionToList(instruction_list,
        createJumpInstruction(TACKY_JUMP, endLabel, NULL));

    addInstructionToList(instruction_list,
        createLabelInstruction(e2Label));
    TACKYValue* elseVal = emit_TACKY(exp->exp.conditional->else_stmt, instruction_list, NULL, symTable);
    addInstructionToList(instruction_list,
        createCopyInstruction(elseVal, copyTackyValue(resultVar)));

    addInstructionToList(instruction_list,
        createLabelInstruction(endLabel));

    return copyTackyValue(resultVar);
}

TACKYValue* emit_TACKYFunctionCall(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    char* funcName = exp->exp.funcCall->identifier;
    ExpressionFactorArray* args = exp->exp.funcCall->arguments;
    TACKYValueArray* tackyArgs = TACKYValueArray_create();
    TACKYValue* resultVar = makeTACKYVariable(getType(exp), symTable);

    // Emit instructions for arguments
    for (int i = 0; i < args->size; i++) {
        CFactor* arg = (CFactor*)args->data[i];
        TACKYValue* argTempVar = makeTACKYVariable(getType(arg), symTable);
        TACKYValue* argVal = emit_TACKY(arg, instruction_list, NULL, symTable);
        addInstructionToList(instruction_list,
            createCopyInstruction(argVal, copyTackyValue(argTempVar)));
        
        TACKYValueArray_append(tackyArgs, argTempVar);
    }

    addInstructionToList(instruction_list,
        createFunCall(funcName, tackyArgs, resultVar));

    return copyTackyValue(resultVar);
}

TACKYInstruction* emitUnaryPostfixInstruction(CFactor* exp, SymbolTable* symTable) {
    if (exp->type != FACTOR_UNARY || !isPostfixUnaryOp(exp->exp.unary->type)) {
        fprintf(stderr, "Error: Expected postfix unary expression\n");
        exit(1);
    }

    char* varName = exp->exp.unary->exp->exp.var->identifier;
    TACKYValue* src = createVarValue(varName);
    TACKYValue* dst = makeTACKYVariable(getType(exp->exp.unary->exp), symTable);
    unaryType op = FromPostPreFixToRegular(exp->exp.unary->type);
    return createUnaryInstruction(op, src, dst);
}

TACKYValue* shortCircuitTACKYInstruction(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    if (exp->type != FACTOR_BINARY) return NULL;

    binType op = exp->exp.binary->type;
    if (op != BIN_AND && op != BIN_OR) return NULL;

    int isPostfixSrc1 = 0, isPostfixSrc2 = 0;
    TACKYValue* leftVal = emit_TACKY(exp->exp.binary->left, instruction_list, &isPostfixSrc1, symTable);
    char* endLabel = generateEndLabel();
    TACKYValue* resultVar = makeTACKYVariable(getType(exp), symTable);
    TACKYInstructionType jumpType = (op == BIN_AND) ? TACKY_JUMP_IF_ZERO : TACKY_JUMP_IF_NOT_ZERO;
    int retVal = (op == BIN_AND) ? 1 : 0;
    char* shortCircuitLabel = (op == BIN_AND) ? generateFalseLabel() : generateTrueLabel();
    addInstructionToList(instruction_list,
        createJumpInstruction(jumpType, shortCircuitLabel, leftVal));

    TACKYValue* rightVal = emit_TACKY(exp->exp.binary->right, instruction_list, &isPostfixSrc2, symTable);
    addInstructionToList(instruction_list,
        createJumpInstruction(jumpType, shortCircuitLabel, rightVal));

    addInstructionToList(instruction_list,
        createCopyInstruction(createTackyValueFromConstant(retVal, 0.0, CONST_INT), resultVar));

    addInstructionToList(instruction_list,
        createJumpInstruction(TACKY_JUMP, endLabel, NULL));

    addInstructionToList(instruction_list,
        createLabelInstruction(shortCircuitLabel));

    addInstructionToList(instruction_list,
        createCopyInstruction(createTackyValueFromConstant(1 - retVal, 0.0, CONST_INT), copyTackyValue(resultVar)));

    addInstructionToList(instruction_list,
        createLabelInstruction(endLabel));
    
    if (isPostfixSrc1) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(exp->exp.binary->left, symTable));
    if (isPostfixSrc2) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(exp->exp.binary->right, symTable));

    return copyTackyValue(resultVar);
}


TACKYValue* emit_TACKYCast(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    TACKYValue* result = emit_TACKY(exp->exp.cast->exp, instruction_list, NULL, symTable);
    specifierType inner_type = getCastSourceType(exp->exp.cast->exp, symTable);
    specifierType t = exp->exp.cast->targetType;
    if (inner_type == t) {
        return result; // No cast needed
    }
    TACKYValue* dst = makeTACKYVariable(t, symTable);
    if (t == SPEC_DOUBLE || inner_type == SPEC_DOUBLE) {
        addInstructionToList(instruction_list,
            generateTACKYDoubleIntCast(result, dst, inner_type, t, symTable));
        return copyTackyValue(dst);
    }
    if (size(t) == size(inner_type)) {
        addInstructionToList(instruction_list,
            createCopyInstruction(result, dst));
    }
    else if (size(t) < size(inner_type)) {
        addInstructionToList(instruction_list,
            createTruncateInstruction(result, dst));
    }
    else if (isSignedSpecifier(inner_type)) {
        addInstructionToList(instruction_list,
            createSignExtendInstruction(result, dst));
    }
    else {
        addInstructionToList(instruction_list,
            createZeroExtendInstruction(result, dst));
    }

    return copyTackyValue(dst);
}