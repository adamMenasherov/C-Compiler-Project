#include "TACKYEmitters.h"
#include "TACKYConstructors.h"
#include "../../generateUtils.h"
#include "../../../Semantic/utils/SemanticUtils/SemanticUtils.h"
#include "../../AST/C-AST-Nodes/C-ASTNodeUtilities/TokenExpect/C-ASTNodeExpect.h"
#include <stdlib.h>
#include <stdio.h>

static CType* getCastSourceType(CFactor* sourceExpr, SymbolTable* symTable) {
    if (!sourceExpr) return NULL;

    switch (sourceExpr->type) {
        case FACTOR_VAR: {
            IdentifierTypeInfo* info = symbolTableLookup(symTable, sourceExpr->exp.var->identifier);
            if (!info) return sourceExpr->valueType;
            return info->type;
        }
        case FACTOR_CONSTANT:
            return constantTypeToCType(sourceExpr->exp.cnst->type);
        case FACTOR_CAST:
            return sourceExpr->exp.cast->targetType;
        default:
            return sourceExpr->valueType;
    }
}

ExpResult* emit_TACKY(CFactor* exp, TACKYInstructionList* instruction_list, int *isPostfixUnary, SymbolTable* symTable) {
    if (!exp) return NULL;
    switch(exp->type) {
        case FACTOR_CONSTANT:
            return createExpResult(EXP_RESULT_PLAIN_OP, createTackyValueFromConstantNode(exp->exp.cnst));
        case FACTOR_VAR:
            return createExpResult(EXP_RESULT_PLAIN_OP, createTackyValueFromVar(exp->exp.var));
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
        case FACTOR_ADDRESS_OF: {
            ExpResult* innerResult = emit_TACKY(exp->exp.pointerOp, instruction_list, NULL, symTable);
            TACKYValue* dst = makeTACKYVariable(getType(exp), symTable);
            addInstructionToList(instruction_list,
                createGetAddressInstruction(innerResult->val, dst));
            return createExpResult(EXP_RESULT_PLAIN_OP, copyTackyValue(dst));
        }
        case FACTOR_DEREFERENCE: {
            TACKYValue* ptrVal = emit_TACKYAndConvert(exp->exp.pointerOp, instruction_list, symTable);
            return createExpResult(EXP_RESULT_DEREF_POINTER_OP, ptrVal);
        }
        default:
            return NULL;
    }
}

ExpResult* emit_TACKYUnary(CFactor* exp, TACKYInstructionList* instruction_list, int *isPostfixUnary, SymbolTable* symTable) {
    if (isPostfixUnaryOp(exp->exp.unary->type)) {
        if (isPostfixUnary) *isPostfixUnary = 1;
        return createExpResult(EXP_RESULT_PLAIN_OP, createTackyValueFromVar(exp->exp.unary->exp->exp.var));
    }

    TACKYValue* src = emit_TACKYAndConvert(exp->exp.unary->exp, instruction_list, symTable);
    TACKYValue* dst = makeTACKYVariable(getType(exp), symTable);
    unaryType op = FromPostPreFixToRegular(exp->exp.unary->type);
    addInstructionToList(instruction_list,
        createUnaryInstruction(op, src, dst));

    return createExpResult(EXP_RESULT_PLAIN_OP, copyTackyValue(dst));
}

ExpResult* emit_TACKYBinary(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    if (exp->exp.binary->type == BIN_AND || exp->exp.binary->type == BIN_OR) {
        return shortCircuitTACKYInstruction(exp, instruction_list, symTable);
    }

    TACKYValue* src1 = emit_TACKYAndConvert(exp->exp.binary->left, instruction_list, symTable);
    TACKYValue* src2 = emit_TACKYAndConvert(exp->exp.binary->right, instruction_list, symTable);
    TACKYValue* dst = makeTACKYVariable(getType(exp), symTable);
    binType op = exp->exp.binary->type;
    addInstructionToList(instruction_list,
        createBinaryInstruction(op, src1, src2, dst));

    return createExpResult(EXP_RESULT_PLAIN_OP, copyTackyValue(dst));
}

ExpResult* emit_TACKYAssignment(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    ExpResult* lhs = emit_TACKY(exp->exp.assignment->exp1, instruction_list, NULL, symTable);
    TACKYValue* src = emit_TACKYAndConvert(exp->exp.assignment->exp2, instruction_list, symTable);

    if (lhs->type == EXP_RESULT_PLAIN_OP) {
        addInstructionToList(instruction_list,
            createCopyInstruction(src, lhs->val));
        return createExpResult(EXP_RESULT_PLAIN_OP, copyTackyValue(src));
    } else {
        addInstructionToList(instruction_list,
            createStoreInstruction(src, lhs->val));
        return createExpResult(EXP_RESULT_PLAIN_OP, copyTackyValue(src));
    }
}

ExpResult* emit_TACKYConditional(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    TACKYValue* cond = emit_TACKYAndConvert(exp->exp.conditional->condition, instruction_list, symTable);
    char* e2Label = generateE2Label();
    char* endLabel = generateEndLabel();
    TACKYValue* resultVar = makeTACKYVariable(getType(exp), symTable);

    addInstructionToList(instruction_list,
        createJumpInstruction(TACKY_JUMP_IF_ZERO, e2Label, cond));

    TACKYValue* thenVal = emit_TACKYAndConvert(exp->exp.conditional->then, instruction_list, symTable);
    addInstructionToList(instruction_list,
        createCopyInstruction(thenVal, copyTackyValue(resultVar)));
    addInstructionToList(instruction_list,
        createJumpInstruction(TACKY_JUMP, endLabel, NULL));

    addInstructionToList(instruction_list,
        createLabelInstruction(e2Label));
    TACKYValue* elseVal = emit_TACKYAndConvert(exp->exp.conditional->else_stmt, instruction_list, symTable);
    addInstructionToList(instruction_list,
        createCopyInstruction(elseVal, copyTackyValue(resultVar)));

    addInstructionToList(instruction_list,
        createLabelInstruction(endLabel));

    return createExpResult(EXP_RESULT_PLAIN_OP, copyTackyValue(resultVar));
}

ExpResult* emit_TACKYFunctionCall(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    char* funcName = exp->exp.funcCall->identifier;
    ExpressionFactorArray* args = exp->exp.funcCall->arguments;
    TACKYValueArray* tackyArgs = TACKYValueArray_create();
    TACKYValue* resultVar = makeTACKYVariable(getType(exp), symTable);

    for (int i = 0; i < args->size; i++) {
        CFactor* arg = (CFactor*)args->data[i];
        TACKYValue* argTempVar = makeTACKYVariable(getType(arg), symTable);
        TACKYValue* argVal = emit_TACKYAndConvert(arg, instruction_list, symTable);
        addInstructionToList(instruction_list,
            createCopyInstruction(argVal, copyTackyValue(argTempVar)));

        TACKYValueArray_append(tackyArgs, argTempVar);
    }

    addInstructionToList(instruction_list,
        createFunCall(funcName, tackyArgs, resultVar));

    return createExpResult(EXP_RESULT_PLAIN_OP, copyTackyValue(resultVar));
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

ExpResult* shortCircuitTACKYInstruction(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    if (exp->type != FACTOR_BINARY) return NULL;

    binType op = exp->exp.binary->type;
    if (op != BIN_AND && op != BIN_OR) return NULL;

    TACKYValue* leftVal = emit_TACKYAndConvert(exp->exp.binary->left, instruction_list, symTable);
    char* endLabel = generateEndLabel();
    TACKYValue* resultVar = makeTACKYVariable(getType(exp), symTable);
    TACKYInstructionType jumpType = (op == BIN_AND) ? TACKY_JUMP_IF_ZERO : TACKY_JUMP_IF_NOT_ZERO;
    int retVal = (op == BIN_AND) ? 1 : 0;
    char* shortCircuitLabel = (op == BIN_AND) ? generateFalseLabel() : generateTrueLabel();
    addInstructionToList(instruction_list,
        createJumpInstruction(jumpType, shortCircuitLabel, leftVal));

    TACKYValue* rightVal = emit_TACKYAndConvert(exp->exp.binary->right, instruction_list, symTable);
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

    return createExpResult(EXP_RESULT_PLAIN_OP, copyTackyValue(resultVar));
}


ExpResult* emit_TACKYCast(CFactor* exp, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    TACKYValue* result = emit_TACKYAndConvert(exp->exp.cast->exp, instruction_list, symTable);
    CType* inner_type = getCastSourceType(exp->exp.cast->exp, symTable);
    CType* t = exp->exp.cast->targetType;
    if (ctypeEqual(inner_type, t)) {
        return createExpResult(EXP_RESULT_PLAIN_OP, result);
    }
    TACKYValue* dst = makeTACKYVariable(t, symTable);
    int tIsDouble     = t && t->kind == CTYPE_DOUBLE;
    int innerIsDouble = inner_type && inner_type->kind == CTYPE_DOUBLE;
    if (tIsDouble || innerIsDouble) {
        addInstructionToList(instruction_list,
            generateTACKYDoubleIntCast(result, dst, inner_type, t, symTable));
        return createExpResult(EXP_RESULT_PLAIN_OP, copyTackyValue(dst));
    }
    if (size(t) == size(inner_type)) {
        addInstructionToList(instruction_list,
            createCopyInstruction(result, dst));
    }
    else if (size(t) < size(inner_type)) {
        addInstructionToList(instruction_list,
            createTruncateInstruction(result, dst));
    }
    else if (isSignedType(inner_type)) {
        addInstructionToList(instruction_list,
            createSignExtendInstruction(result, dst));
    }
    else {
        addInstructionToList(instruction_list,
            createZeroExtendInstruction(result, dst));
    }

    return createExpResult(EXP_RESULT_PLAIN_OP, copyTackyValue(dst));
}

TACKYValue* emit_TACKYAndConvert(CFactor* val, TACKYInstructionList* instruction_list, SymbolTable* symTable) {
    int isPostfixUnary = 0;
    ExpResult* result = emit_TACKY(val, instruction_list, &isPostfixUnary, symTable);

    TACKYValue* resolved;
    switch(result->type) {
        case EXP_RESULT_PLAIN_OP:
            resolved = result->val;
            break;
        case EXP_RESULT_DEREF_POINTER_OP: {
            TACKYValue* dst = makeTACKYVariable(getType(val), symTable);
            addInstructionToList(instruction_list,
                createLoadInstruction(result->val, dst));
            resolved = dst;
            break;
        }
        default:
            resolved = result->val;
    }

    if (isPostfixUnary) addInstructionToList(instruction_list,
        emitUnaryPostfixInstruction(val, symTable));

    return resolved;
}
