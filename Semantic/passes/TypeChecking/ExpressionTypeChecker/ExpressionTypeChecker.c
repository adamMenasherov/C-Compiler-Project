#include "ExpressionTypeChecker.h"
#include "../../../utils/SemanticUtils/SemanticUtils.h"
#include "../../../../Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodeConstructors.h"
#include <stdio.h>
#include <stdlib.h>

static inline int getFunctionParamCount(const CFuncType* funcType) {
    return funcType->func.paramCnt;
}

static CFactor* convertTo(CFactor* expr, specifierType targetType) {
    if (expr->valueType == targetType) return expr;
    CCast* castNode = C_CreateCast(targetType, expr);
    CFactor* castExpr = C_CreateFactorFromCast(castNode);
    if (!castExpr) {
        fprintf(stderr, "Semantic Error: Failed to create cast expression\n");
        exit(1);
    }
    setType(castExpr, targetType);
    return castExpr;
}


static void handleTypeCheckFuncCall(CFactor* expr, SymbolTable* symbolTable) {
    IdentifierTypeInfo* info = symbolTableLookup(symbolTable, expr->exp.funcCall->identifier);
    if (!info) {
        fprintf(stderr, "Semantic Error: Undeclared function '%s'\n", fromTempToOrigin(expr->exp.funcCall->identifier));
        exit(1);
    }
    if (info->type != TYPE_FUNCTION) {
        fprintf(stderr, "Semantic Error: '%s' is not a function\n", fromTempToOrigin(expr->exp.funcCall->identifier));
        exit(1);
    }
    ExpressionFactorArray* args = expr->exp.funcCall->arguments;
    if (ExpressionFactorArray_size(args) != getFunctionParamCount(info->funcInfo.funcType)) {
        fprintf(stderr, "Semantic Error: Function '%s' called with incorrect number of arguments\n", fromTempToOrigin(expr->exp.funcCall->identifier));
        exit(1);
    }
    for (int i = 0; i < args->size; i++) {
        CFactor* arg = (CFactor*)args->data[i];
        typeCheckExpression(arg, symbolTable);
        specifierType paramType = info->funcInfo.funcType->func.params[i]->type;
        args->data[i] = convertTo(arg, paramType);
    }
    setType(expr, info->funcInfo.funcType->func.ret->type);
}

static void handleTypeCheckVar(CFactor* expr, SymbolTable* symbolTable) {
    IdentifierTypeInfo* info = symbolTableLookup(symbolTable, expr->exp.var->identifier);
    if (!info) {
        fprintf(stderr, "Semantic Error: Undeclared variable '%s'\n", fromTempToOrigin(expr->exp.var->identifier));
        exit(1);
    }
    if (info->type == TYPE_FUNCTION) {
        fprintf(stderr, "Semantic Error: '%s' is a function, not a variable\n", fromTempToOrigin(expr->exp.var->identifier));
        exit(1);
    }
    setTypeVar(expr, info->type);
}

static void handleTypeCheckUnary(CFactor* expr, SymbolTable* symbolTable) {
    typeCheckExpression(expr->exp.unary->exp, symbolTable);
    if (expr->exp.unary->type == UNARY_NOT) 
        setType(expr, SPEC_INT); // Evaluates to an int (0 or 1)
    else 
        setType(expr, expr->exp.unary->exp->valueType);
}

static void handleTypeCheckBinary(CFactor* expr, SymbolTable* symbolTable) {
    typeCheckExpression(expr->exp.binary->left, symbolTable);
    typeCheckExpression(expr->exp.binary->right, symbolTable);

    if (expr->exp.binary->type == BIN_AND || expr->exp.binary->type == BIN_OR) {
        setType(expr, SPEC_INT); 
    }
    else {
        specifierType leftType = expr->exp.binary->left->valueType;
        specifierType rightType = expr->exp.binary->right->valueType;
        specifierType commonType = getCommonType(leftType, rightType);
        expr->exp.binary->left = convertTo(expr->exp.binary->left, commonType);
        expr->exp.binary->right = convertTo(expr->exp.binary->right, commonType);
        setType(expr, commonType);
    }
}

static void handleTypeCheckAssignment(CFactor* expr, SymbolTable* symbolTable) {
    if (expr->exp.assignment->exp1->type != FACTOR_VAR) {
        fprintf(stderr, "Semantic Error: Left-hand side of assignment must be a variable\n");
        exit(1);
    }
    typeCheckExpression(expr->exp.assignment->exp1, symbolTable);
    typeCheckExpression(expr->exp.assignment->exp2, symbolTable);
    specifierType varType = expr->exp.assignment->exp1->valueType;
    expr->exp.assignment->exp2 = convertTo(expr->exp.assignment->exp2, varType);
    setType(expr, varType);
}

static void handleTypeCheckConditional(CFactor* expr, SymbolTable* symbolTable) {
    typeCheckExpression(expr->exp.conditional->condition, symbolTable);
    typeCheckExpression(expr->exp.conditional->then, symbolTable);
    typeCheckExpression(expr->exp.conditional->else_stmt, symbolTable);
    specifierType thenType = expr->exp.conditional->then->valueType;
    specifierType elseType = expr->exp.conditional->else_stmt->valueType;
    specifierType commonType = getCommonType(thenType, elseType);
    expr->exp.conditional->then = convertTo(expr->exp.conditional->then, commonType);
    expr->exp.conditional->else_stmt = convertTo(expr->exp.conditional->else_stmt, commonType);
    setType(expr, commonType);
}

static void handleTypeCheckConstant(CFactor* expr, SymbolTable* symbolTable) {
    setTypeConst(expr, expr->exp.cnst->type);
}

static void handleTypeCheckCast(CFactor* expr, SymbolTable* symbolTable) {
    typeCheckExpression(expr->exp.cast->exp, symbolTable);
    setType(expr, expr->exp.cast->targetType);
}

typedef void (*ExprTypeChecker)(CFactor*, SymbolTable*);

static const ExprTypeChecker exprTypeCheckers[] = {
    [FACTOR_FUNCTION_CALL] = handleTypeCheckFuncCall,
    [FACTOR_VAR]           = handleTypeCheckVar,
    [FACTOR_UNARY]         = handleTypeCheckUnary,
    [FACTOR_CAST]          = handleTypeCheckCast, 
    [FACTOR_BINARY]        = handleTypeCheckBinary,
    [FACTOR_ASSIGNMENT]    = handleTypeCheckAssignment,
    [FACTOR_CONDITIONAL]   = handleTypeCheckConditional,
    [FACTOR_CONSTANT]      = handleTypeCheckConstant,
};

void typeCheckExpression(CFactor* expr, SymbolTable* symbolTable) {
    if (!expr) return;
    ExprTypeChecker handler = exprTypeCheckers[expr->type];
    if (!handler) return;
    handler(expr, symbolTable);
}
