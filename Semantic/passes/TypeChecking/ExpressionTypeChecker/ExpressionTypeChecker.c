#include "ExpressionTypeChecker.h"
#include "../../../utils/SemanticUtils/SemanticUtils.h"
#include "../../../../Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodeConstructors.h"
#include <stdio.h>
#include <stdlib.h>

static void handleTypeCheckDereference(CFactor* expr, SymbolTable* symbolTable) {
    typeCheckExpression(expr->exp.pointerOp, symbolTable);
    if (!expr->exp.pointerOp->valueType || expr->exp.pointerOp->valueType->kind != CTYPE_POINTER) {
        fprintf(stderr, "Semantic Error: Cannot dereference non-pointer type\n");
        exit(1);
    }
    setType(expr, expr->exp.pointerOp->valueType->pointer.referenced);
}


static void handleTypeCheckAddressOf(CFactor* expr, SymbolTable* symbolTable) {
    typeCheckExpression(expr->exp.pointerOp, symbolTable);
    if (!isLvalue(expr->exp.pointerOp)) {
        fprintf(stderr, "Semantic Error: Cannot take address of a non-lvalue\n");
        exit(1);
    }
    setType(expr, C_CreatePointerType(expr->exp.pointerOp->valueType));
}


static void handleTypeCheckFuncCall(CFactor* expr, SymbolTable* symbolTable) {
    IdentifierTypeInfo* info = symbolTableLookup(symbolTable, expr->exp.funcCall->identifier);
    if (!info) {
        fprintf(stderr, "Semantic Error: Undeclared function '%s'\n", fromTempToOrigin(expr->exp.funcCall->identifier));
        exit(1);
    }
    if (!info->type || info->type->kind != CTYPE_FUN) {
        fprintf(stderr, "Semantic Error: '%s' is not a function\n", fromTempToOrigin(expr->exp.funcCall->identifier));
        exit(1);
    }
    ExpressionFactorArray* args = expr->exp.funcCall->arguments;
    if (ExpressionFactorArray_size(args) != getFunctionParamCount(info->type)) {
        fprintf(stderr, "Semantic Error: Function '%s' called with incorrect number of arguments\n", fromTempToOrigin(expr->exp.funcCall->identifier));
        exit(1);
    }
    for (int i = 0; i < args->size; i++) {
        CFactor* arg = (CFactor*)args->data[i];
        typeCheckExpression(arg, symbolTable);
        CType* paramType = info->type->fun.params[i];
        args->data[i] = convertTo(arg, paramType);
    }
    setType(expr, info->type->fun.ret);
}

static void handleTypeCheckVar(CFactor* expr, SymbolTable* symbolTable) {
    IdentifierTypeInfo* info = symbolTableLookup(symbolTable, expr->exp.var->identifier);
    if (!info) {
        fprintf(stderr, "Semantic Error: Undeclared variable '%s'\n", fromTempToOrigin(expr->exp.var->identifier));
        exit(1);
    }
    if (info->type && info->type->kind == CTYPE_FUN) {
        fprintf(stderr, "Semantic Error: '%s' is a function, not a variable\n", fromTempToOrigin(expr->exp.var->identifier));
        exit(1);
    }
    setTypeVar(expr, info->type);
}

static void handleTypeCheckUnary(CFactor* expr, SymbolTable* symbolTable) {
    typeCheckExpression(expr->exp.unary->exp, symbolTable);
    if (expr->exp.unary->exp->valueType &&
        getType(expr->exp.unary->exp)->kind == CTYPE_DOUBLE &&
        expr->exp.unary->type == UNARY_COMPLEMENT) {
        fprintf(stderr, "Semantic Error: ~ not supported on double type\n");
        exit(1);
    }
    if (getType(expr->exp.unary->exp)->kind == CTYPE_POINTER &&
        (expr->exp.unary->type == UNARY_NEGATE || expr->exp.unary->type == UNARY_NOT)) {
        fprintf(stderr, "Semantic Error: Unary - and ! not supported on pointer types\n");
        exit(1);
    }
    if (expr->exp.unary->type == UNARY_NOT)
        setType(expr, C_CreateType(CTYPE_INT));
    else
        setType(expr, expr->exp.unary->exp->valueType);
}

static void handleTypeCheckBinary(CFactor* expr, SymbolTable* symbolTable) {
    typeCheckExpression(expr->exp.binary->left, symbolTable);
    typeCheckExpression(expr->exp.binary->right, symbolTable);

    CType* commonType;
    CType* leftType  = expr->exp.binary->left->valueType;
    CType* rightType = expr->exp.binary->right->valueType;

    int leftIsPtr  = leftType  && leftType->kind  == CTYPE_POINTER;
    int rightIsPtr = rightType && rightType->kind == CTYPE_POINTER;
    int leftIsInt  = leftType  && isArithmeticType(leftType) && leftType->kind != CTYPE_DOUBLE;
    int rightIsInt = rightType && isArithmeticType(rightType) && rightType->kind != CTYPE_DOUBLE;

    if (leftIsPtr && rightIsInt) {
        commonType = leftType;
    } else if (rightIsPtr && leftIsInt) {
        commonType = rightType;
    } else if (leftIsPtr || rightIsPtr) {
        commonType = getCommonPointerType(expr->exp.binary->left, expr->exp.binary->right);
        if (!commonType) {
            fprintf(stderr, "Semantic Error: Incompatible pointer types in binary expression\n");
            exit(1);
        }
        expr->exp.binary->left  = convertTo(expr->exp.binary->left,  commonType);
        expr->exp.binary->right = convertTo(expr->exp.binary->right, commonType);
    } else {
        commonType = getCommonType(leftType, rightType);
        expr->exp.binary->left  = convertTo(expr->exp.binary->left,  commonType);
        expr->exp.binary->right = convertTo(expr->exp.binary->right, commonType);
    }

    if (isRelationBinaryOp(expr->exp.binary->type)) {
        setType(expr, C_CreateType(CTYPE_INT));
    }
    else {
        binType op = expr->exp.binary->type;
        if ((op != BIN_ADD && op != BIN_SUBTRACT) && commonType && commonType->kind == CTYPE_POINTER) {
            fprintf(stderr, "Semantic Error: Only + and - operators supported for pointer types\n");
            exit(1);
        }
        if (op == BIN_REMAINDER && commonType && commonType->kind == CTYPE_DOUBLE) {
            fprintf(stderr, "Semantic Error: Modulo operator not supported for double type\n");
            exit(1);
        }
        setType(expr, commonType);
    }
}

static void handleTypeCheckAssignment(CFactor* expr, SymbolTable* symbolTable) {
    if (!isLvalue(expr->exp.assignment->exp1)) {
        fprintf(stderr, "Semantic Error: Left-hand side of assignment must be an lvalue\n");
        exit(1);
    }
    typeCheckExpression(expr->exp.assignment->exp1, symbolTable);
    typeCheckExpression(expr->exp.assignment->exp2, symbolTable);
    CType* varType = expr->exp.assignment->exp1->valueType;
    expr->exp.assignment->exp2 = convertByAssignment(expr->exp.assignment->exp2, varType);
    setType(expr, varType);
}

static void handleTypeCheckConditional(CFactor* expr, SymbolTable* symbolTable) {
    typeCheckExpression(expr->exp.conditional->condition, symbolTable);
    typeCheckExpression(expr->exp.conditional->then, symbolTable);
    typeCheckExpression(expr->exp.conditional->else_stmt, symbolTable);
    CType* thenType = expr->exp.conditional->then->valueType;
    CType* elseType = expr->exp.conditional->else_stmt->valueType;
    CType* commonType = getCommonType(thenType, elseType);
    expr->exp.conditional->then      = convertTo(expr->exp.conditional->then,      commonType);
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
    [FACTOR_DEREFERENCE]   = handleTypeCheckDereference,
    [FACTOR_ADDRESS_OF]    = handleTypeCheckAddressOf
};

void typeCheckExpression(CFactor* expr, SymbolTable* symbolTable) {
    if (!expr) return;
    ExprTypeChecker handler = exprTypeCheckers[expr->type];
    if (!handler) return;
    handler(expr, symbolTable);
}
