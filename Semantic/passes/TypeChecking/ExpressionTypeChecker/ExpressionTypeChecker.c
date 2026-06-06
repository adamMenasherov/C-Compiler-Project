#include "ExpressionTypeChecker.h"
#include "../../../utils/SemanticUtils/SemanticUtils.h"
#include "../../../../Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodeConstructors.h"
#include <stdio.h>
#include <stdlib.h>

static void handleTypeCheckDereference(CFactor* expr, SymbolTable* symbolTable) {
    expr->exp.pointerOp = typeCheckAndConvert(expr->exp.pointerOp, symbolTable);
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
        CFactor* arg = typeCheckAndConvert((CFactor*)args->data[i], symbolTable);
        CType* paramType = info->type->fun.params[i];
        args->data[i] = convertByAssignment(arg, paramType);
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
    expr->exp.unary->exp = typeCheckAndConvert(expr->exp.unary->exp, symbolTable);
    if (expr->exp.unary->exp->valueType &&
        getType(expr->exp.unary->exp)->kind == CTYPE_DOUBLE &&
        expr->exp.unary->type == UNARY_COMPLEMENT) {
        fprintf(stderr, "Semantic Error: ~ not supported on double type\n");
        exit(1);
    }
    if (getType(expr->exp.unary->exp)->kind == CTYPE_POINTER &&
        (expr->exp.unary->type == UNARY_NEGATE || expr->exp.unary->type == UNARY_COMPLEMENT)) {
        fprintf(stderr, "Semantic Error: Unary - and ~ not supported on pointer types\n");
        exit(1);
    }
    if (expr->exp.unary->type == UNARY_NOT)
        setType(expr, C_CreateType(CTYPE_INT));
    else
        setType(expr, expr->exp.unary->exp->valueType);
}

static CType* handlePointerArithmetic(CFactor* expr, CType* leftType, CType* rightType, int leftIsPtr, int rightIsPtr,
                                       int leftIsInt, int rightIsInt) 
{
    binType bop = expr->exp.binary->type;
    if (bop == BIN_ADD) {
        if (leftIsPtr && rightIsInt) {
            expr->exp.binary->right = convertTo(expr->exp.binary->right, C_CreateType(CTYPE_LONG));
            return leftType;
        }
        if (rightIsPtr && leftIsInt) {
            expr->exp.binary->left = convertTo(expr->exp.binary->left, C_CreateType(CTYPE_LONG));
            return rightType;
        }
    }
    if (bop == BIN_SUBTRACT) {
        if (leftIsPtr && rightIsInt) {
            expr->exp.binary->right = convertTo(expr->exp.binary->right, C_CreateType(CTYPE_LONG));
            return leftType;
        }
        if (leftIsPtr && rightIsPtr) {
            if (!ctypeEqual(leftType, rightType)) {
                fprintf(stderr, "Semantic Error: Pointer subtraction requires identical pointer types\n");
                exit(1);
            }
            return C_CreateType(CTYPE_LONG);
        }
        if (rightIsPtr) {
            fprintf(stderr, "Semantic Error: Cannot subtract pointer from integer\n");
            exit(1);
        }
    }

    // Pointers don't support other binary ops
    if (leftIsPtr || rightIsPtr) { 
        fprintf(stderr, "Semantic Error: Invalid operands for binary operation\n");
        exit(1);
    }

    return NULL; // not a pointer operation
}

static void handleRelational(CFactor* expr, CType* leftType, CType* rightType,
                              int leftIsPtr, int rightIsPtr) {
    if (leftIsPtr || rightIsPtr) {
        int leftIsNull  = isNullPointerConstant(expr->exp.binary->left);
        int rightIsNull = isNullPointerConstant(expr->exp.binary->right);
        if ((leftIsPtr && !rightIsPtr && !rightIsNull) ||
            (rightIsPtr && !leftIsPtr && !leftIsNull)) {
            fprintf(stderr, "Semantic Error: Cannot compare pointer with non-null integer\n");
            exit(1);
        }
        if (leftIsPtr && rightIsPtr) {
            CType* common = getCommonPointerType(expr->exp.binary->left, expr->exp.binary->right);
            if (!common) {
                fprintf(stderr, "Semantic Error: Incompatible pointer types in comparison\n");
                exit(1);
            }
            expr->exp.binary->left = convertTo(expr->exp.binary->left,  common);
            expr->exp.binary->right = convertTo(expr->exp.binary->right, common);
        }
    } else {
        CType* common = getCommonType(leftType, rightType);
        expr->exp.binary->left = convertTo(expr->exp.binary->left,  common);
        expr->exp.binary->right = convertTo(expr->exp.binary->right, common);
    }
    setType(expr, C_CreateType(CTYPE_INT));
}

static void handleTypeCheckSubscript(CFactor* expr, SymbolTable* symbolTable) {
    expr->exp.subscript->pointer = typeCheckAndConvert(expr->exp.subscript->pointer, symbolTable);
    expr->exp.subscript->index = typeCheckAndConvert(expr->exp.subscript->index, symbolTable);
    CType* t1 = getType(expr->exp.subscript->pointer);
    CType* t2 = getType(expr->exp.subscript->index);
    CType* ptrType;
    int leftIsPtr  = t1  && t1->kind  == CTYPE_POINTER;
    int rightIsPtr = t2 && t2->kind == CTYPE_POINTER;
    int leftIsInt  = t1  && isArithmeticType(t1)  && t1->kind != CTYPE_DOUBLE;
    int rightIsInt = t2 && isArithmeticType(t2) && t2->kind != CTYPE_DOUBLE;

    if (leftIsPtr && rightIsInt) {
        ptrType = t1;
        expr->exp.subscript->index = convertTo(expr->exp.subscript->index, C_CreateType(CTYPE_LONG));
    }
    else if (leftIsInt && rightIsPtr) {
        ptrType = t2;
        expr->exp.subscript->pointer = convertTo(expr->exp.subscript->pointer, C_CreateType(CTYPE_LONG));
    }
    else {
        fprintf(stderr, "Semantic Error: Subscript operator requires one pointer and one integer operand\n");
        exit(1);
    }
    setType(expr, ptrType->pointer.referenced);
}

static void handleTypeCheckBinary(CFactor* expr, SymbolTable* symbolTable) {
    expr->exp.binary->left  = typeCheckAndConvert(expr->exp.binary->left,  symbolTable);
    expr->exp.binary->right = typeCheckAndConvert(expr->exp.binary->right, symbolTable);
    binType bop = expr->exp.binary->type;

    if (bop == BIN_AND || bop == BIN_OR) {
        setType(expr, C_CreateType(CTYPE_INT));
        return;
    }
    CType* leftType  = expr->exp.binary->left->valueType;
    CType* rightType = expr->exp.binary->right->valueType;
    int leftIsPtr  = leftType  && leftType->kind  == CTYPE_POINTER;
    int rightIsPtr = rightType && rightType->kind == CTYPE_POINTER;
    int leftIsInt  = leftType  && isArithmeticType(leftType)  && leftType->kind != CTYPE_DOUBLE;
    int rightIsInt = rightType && isArithmeticType(rightType) && rightType->kind != CTYPE_DOUBLE;
    CType* ptrResult = handlePointerArithmetic(expr, leftType, rightType,
                                               leftIsPtr, rightIsPtr, leftIsInt, rightIsInt);
    if (ptrResult) {
         setType(expr, ptrResult); 
         return;
    }
    if (isRelationBinaryOp(bop)) { 
        handleRelational(expr, leftType, rightType, leftIsPtr, rightIsPtr); 
        return; 
    }
    if (bop == BIN_REMAINDER && ((leftType && leftType->kind == CTYPE_DOUBLE) ||
                                  (rightType && rightType->kind == CTYPE_DOUBLE))) {
        fprintf(stderr, "Semantic Error: Modulo operator not supported for double type\n");
        exit(1);
    }

    CType* common = getCommonType(leftType, rightType);
    expr->exp.binary->left = convertTo(expr->exp.binary->left,  common);
    expr->exp.binary->right = convertTo(expr->exp.binary->right, common);
    setType(expr, common);
}

static void handleTypeCheckAssignment(CFactor* expr, SymbolTable* symbolTable) {
    expr->exp.assignment->exp1 = typeCheckAndConvert(expr->exp.assignment->exp1, symbolTable);
    if (!isLvalue(expr->exp.assignment->exp1)) {
        fprintf(stderr, "Semantic Error: Left-hand side of assignment must be an lvalue\n");
        exit(1);
    }
    expr->exp.assignment->exp2 = typeCheckAndConvert(expr->exp.assignment->exp2, symbolTable);
    CType* varType = expr->exp.assignment->exp1->valueType;
    expr->exp.assignment->exp2 = convertByAssignment(expr->exp.assignment->exp2, varType);
    setType(expr, varType);
}

static void handleTypeCheckConditional(CFactor* expr, SymbolTable* symbolTable) {
    expr->exp.conditional->condition  = typeCheckAndConvert(expr->exp.conditional->condition,  symbolTable);
    expr->exp.conditional->then = typeCheckAndConvert(expr->exp.conditional->then,       symbolTable);
    expr->exp.conditional->else_stmt = typeCheckAndConvert(expr->exp.conditional->else_stmt,  symbolTable);
    CType* thenType = expr->exp.conditional->then->valueType;
    CType* elseType = expr->exp.conditional->else_stmt->valueType;
    CType* commonType = getCommonType(thenType, elseType);
    expr->exp.conditional->then = convertTo(expr->exp.conditional->then,      commonType);
    expr->exp.conditional->else_stmt = convertTo(expr->exp.conditional->else_stmt, commonType);
    setType(expr, commonType);
}

static void handleTypeCheckConstant(CFactor* expr, SymbolTable* symbolTable) {
    setTypeConst(expr, expr->exp.cnst->type);
}

static void handleTypeCheckCast(CFactor* expr, SymbolTable* symbolTable) {
    expr->exp.cast->exp = typeCheckAndConvert(expr->exp.cast->exp, symbolTable);
    if (expr->exp.cast->targetType->kind == CTYPE_ARRAY) {
        fprintf(stderr, "Semantic Error: Cannot cast to array type\n");
        exit(1);
    }
    CType* srcType = expr->exp.cast->exp->valueType;
    CType* dstType = expr->exp.cast->targetType;
    if (srcType && dstType) {
        if (srcType->kind == CTYPE_POINTER && dstType->kind == CTYPE_DOUBLE) {
            fprintf(stderr, "Semantic Error: Cannot cast pointer to double\n");
            exit(1);
        }
        if (srcType->kind == CTYPE_DOUBLE && dstType->kind == CTYPE_POINTER) {
            fprintf(stderr, "Semantic Error: Cannot cast double to pointer\n");
            exit(1);
        }
    }
    setType(expr, dstType);
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
    [FACTOR_ADDRESS_OF]    = handleTypeCheckAddressOf,
    [FACTOR_SUBSCRIPT]     = handleTypeCheckSubscript
};

void typeCheckExpression(CFactor* expr, SymbolTable* symbolTable) {
    if (!expr) return;
    ExprTypeChecker handler = exprTypeCheckers[expr->type];
    if (!handler) return;
    handler(expr, symbolTable);
}

CFactor* typeCheckAndConvert(CFactor* expr, SymbolTable* symbolTable) {
    if (!expr) return NULL;
    typeCheckExpression(expr, symbolTable);
    CType* type = getType(expr);
    if (!type) return expr;
    if (type->kind == CTYPE_ARRAY) {
        // Array-to-pointer decay (with the base type)
        CType* ptrType = C_CreatePointerType(type->array.elementType);
        CFactor* addrOf = C_CreateAddrOfNode(expr);
        setType(addrOf, ptrType);
        return addrOf;
    }
    return expr;
}


void typeCheckInit(CType* targetType, CInitializer* initializer, SymbolTable* symbolTable) {
    if (initializer->type == INIT_SINGLE) {
        initializer->init.singleInit = typeCheckAndConvert(initializer->init.singleInit, symbolTable);
        initializer->init.singleInit = convertByAssignment(initializer->init.singleInit, targetType);
        setType(initializer->init.singleInit, targetType);
    } 
    else if (initializer->type == INIT_COMPOUND && targetType->kind == CTYPE_ARRAY) 
    {
        if (initializer->init.compoundInit.initializers->size > targetType->array.size) {
            fprintf(stderr, "Semantic Error: Too many initializers for array of size %d\n", targetType->array.size);
            exit(1);
        }
        for (int i = 0; i < initializer->init.compoundInit.initializers->size; i++) {
            CInitializer* elemInit = (CInitializer*)initializer->init.compoundInit.initializers->data[i];
            typeCheckInit(targetType->array.elementType, elemInit, symbolTable);
        }
        while (initializer->init.compoundInit.initializers->size < targetType->array.size) {
            CInitializer* zeroInit = C_CreateSingleInit(
                C_CreateFactorFromConstant(C_CreateConstant(0, 0, CONST_INT)));
            CInitializerList_append(initializer->init.compoundInit.initializers, zeroInit);
        }
        return;
    }
    else {
        fprintf(stderr, "Semantic Error: Invalid initializer for target type\n");
        exit(1);
    }
    
}