#include "SemanticUtils.h"
#include "Parser/generateUtils.h"
#include "Parser/Common/SharedTypeRank.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* generateUniqueVariableName(char* baseName) {
    char* uniqueName = malloc(strlen(baseName) + 20);
    if (!uniqueName) return NULL;
    sprintf(uniqueName, "%s.%d", baseName, currGlobalInt++);
    return uniqueName;
}

const char* fromTempToOrigin(char* identifier) {
    return (const char*)strtok(identifier, ".");
}

void setTypeVar(CFactor* expr, CType* type) {
    expr->valueType = type;
}

void setType(CFactor* expr, CType* type) {
    expr->valueType = type;
}

void setTypeConst(CFactor* expr, constantType type) {
    expr->valueType = constantTypeToCType(type);
}

int isArithmeticType(CType* type) {
    return type->kind != CTYPE_POINTER && type->kind != CTYPE_FUN;
}

int ctypeEqual(CType* a, CType* b) {
    if (!a || !b) return a == b;
    if (a->kind != b->kind) return 0;
    switch (a->kind) {
        case CTYPE_INT:
        case CTYPE_LONG:
        case CTYPE_UINT:
        case CTYPE_ULONG:
        case CTYPE_DOUBLE:
            return 1;
        case CTYPE_POINTER:
            return ctypeEqual(a->pointer.referenced, b->pointer.referenced);
        case CTYPE_FUN: {
            if (a->fun.paramCnt != b->fun.paramCnt) return 0;
            if (!ctypeEqual(a->fun.ret, b->fun.ret)) return 0;
            for (int i = 0; i < a->fun.paramCnt; i++) {
                if (!ctypeEqual(a->fun.params[i], b->fun.params[i])) return 0;
            }
            return 1;
        }
        default:
            return 0;
    }
}

CType* getCommonType(CType* type1, CType* type2) {
    if (ctypeEqual(type1, type2)) return type1;
    if (type1->kind == CTYPE_DOUBLE || type2->kind == CTYPE_DOUBLE) return C_CreateType(CTYPE_DOUBLE);
    if (size(type1) == size(type2)) {
        if (isSignedType(type1)) return type2;
        else return type1;
    }
    if (size(type1) > size(type2)) return type1;
    else return type2;
}

CType* getCommonPointerType(CFactor* exp1, CFactor* exp2) {
    CType* t1 = getType(exp1);
    CType* t2 = getType(exp2);
    if (ctypeEqual(t1, t2)) return t1;
    else if (isNullPointerConstant(exp1)) return t2;
    else if (isNullPointerConstant(exp2)) return t1;
    else return NULL;
}


int isBasicType(CType* type) {
    if (!type) return 0;
    return type->kind == CTYPE_INT || type->kind == CTYPE_LONG ||
           type->kind == CTYPE_UINT || type->kind == CTYPE_ULONG || type->kind == CTYPE_DOUBLE
           || type->kind == CTYPE_POINTER;
}

initialValueStaticInitType convertExpTypeToStaticInitType(CType* expType) {
    if (!expType) { fprintf(stderr, "Semantic Error: NULL type in convertExpTypeToStaticInitType\n"); exit(1); }
    switch (expType->kind) {
        case CTYPE_INT:  return STATIC_INIT_INT;
        case CTYPE_LONG: return STATIC_INIT_LONG;
        default:
            fprintf(stderr, "Semantic Error: Unsupported initializer type for file scope variable\n");
            exit(1);
    }
}

CType* getType(CFactor* factor) {
    return factor->valueType;
}

int isSignedCType(CType* type) {
    if (!type) return 0;
    return type->kind == CTYPE_INT || type->kind == CTYPE_LONG;
}

void convertValFromType(uint64_t* val, CType* toType) {
    if (!toType) { fprintf(stderr, "Invalid type in convertValFromType\n"); exit(1); }
    switch (toType->kind) {
        case CTYPE_INT:
            *val = (int)(*val);
            return;
        case CTYPE_LONG:
            *val = (long)(*val);
            return;
        case CTYPE_UINT:
            *val = (unsigned int)(*val);
            return;
        case CTYPE_ULONG:
            *val = (unsigned long)(*val);
            return;
        case CTYPE_POINTER:
            if (*val != 0 && toType->kind == CTYPE_POINTER) {
                fprintf(stderr, "Semantic Error: Cannot initialize pointer with non-zero integer (not null)\n");
                exit(1);
            }
            *val = (unsigned long)(*val);
            return;
        case CTYPE_DOUBLE:
            return;
        default:
            fprintf(stderr, "Invalid CType kind in convertValFromType\n");
            exit(1);
    }
}

initialValueStaticInitType convertSpecTypeToStaticInitType(CType* type) {
    if (!type) { fprintf(stderr, "Semantic Error: NULL type in convertSpecTypeToStaticInitType\n"); exit(1); }
    switch (type->kind) {
        case CTYPE_INT:    return STATIC_INIT_INT;
        case CTYPE_LONG:   return STATIC_INIT_LONG;
        case CTYPE_UINT:   return STATIC_INIT_UNSIGNED_INT;
        case CTYPE_ULONG:  return STATIC_INIT_UNSIGNED_LONG;
        case CTYPE_DOUBLE: return STATIC_INIT_DOUBLE;
        default:
            fprintf(stderr, "Semantic Error: Unsupported variable type for static initialization\n");
            exit(1);
    }
}

int isNullPointerConstant(CFactor* factor) {
    switch(factor->type) {
        case FACTOR_CONSTANT:
            if (factor->exp.cnst->value.intValue != 0) return 0; 
            switch (factor->exp.cnst->type) {
                case CONST_INT: 
                case CONST_LONG:
                case CONST_UNSIGNED_INT:
                case CONST_UNSIGNED_LONG:
                    return 1;
                case CONST_FLOATING_POINT: 
                    return 0;
                default: return 0;
            }
    }
}


int isLvalue(const CFactor* expr) {
    return expr->type == FACTOR_VAR || expr->type == FACTOR_DEREFERENCE;
}

int getFunctionParamCount(const CType* funcType) {
    return funcType->fun.paramCnt;
}

CFactor* convertTo(CFactor* expr, CType* targetType) {
    if (ctypeEqual(expr->valueType, targetType)) return expr;
    if (getType(expr)->kind == CTYPE_POINTER && targetType->kind == CTYPE_DOUBLE) {
        fprintf(stderr, "Semantic Error: Cannot convert pointer type to double\n");
        exit(1);
    }
    if (getType(expr)->kind == CTYPE_DOUBLE && targetType->kind == CTYPE_POINTER) {
        fprintf(stderr, "Semantic Error: Cannot convert double type to pointer\n");
        exit(1);
    }
    CCast* castNode = C_CreateCast(targetType, expr);
    CFactor* castExpr = C_CreateFactorFromCast(castNode);
    if (!castExpr) {
        fprintf(stderr, "Semantic Error: Failed to create cast expression\n");
        exit(1);
    }
    setType(castExpr, targetType);
    return castExpr;
}

CFactor* convertByAssignment(CFactor* expr, CType* targetType) {
    if (ctypeEqual(expr->valueType, targetType)) return expr;
    if (isArithmeticType(expr->valueType) && isArithmeticType(targetType)) {
        return convertTo(expr, targetType);
    }
    if (isNullPointerConstant(expr) && targetType->kind == CTYPE_POINTER) {
        return convertTo(expr, targetType);
    }
    else {
        fprintf(stderr, "Semantic Error: Cannot convert type for assignment\n");
        exit(1);
    }
}