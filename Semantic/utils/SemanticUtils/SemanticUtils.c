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

int isBasicType(CType* type) {
    if (!type) return 0;
    return type->kind == CTYPE_INT || type->kind == CTYPE_LONG ||
           type->kind == CTYPE_UINT || type->kind == CTYPE_ULONG;
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
