#include "SharedTypeRank.h"
#include "DataStructures/HashTable/Wrappers/SymbolTableWrapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int size(CType* type) {
    if (!type) { fprintf(stderr, "size: NULL type\n"); exit(1); }
    switch (type->kind) {
        case CTYPE_INT:
        case CTYPE_UINT:
            return 4;
        case CTYPE_LONG:
        case CTYPE_ULONG:
        case CTYPE_DOUBLE:
        case CTYPE_POINTER:
            return 8;
        case CTYPE_ARRAY:
            return size(type->array.elementType) * type->array.size;
        default:
            fprintf(stderr, "Invalid CType kind in size function\n");
            exit(1);
    }
}

int isSignedType(CType* type) {
    if (!type) return 0;
    return type->kind == CTYPE_INT || type->kind == CTYPE_LONG;
}

int isDoubleLabel(const char* identifier) {
    return identifier && strncmp(identifier, ".LC", 3) == 0;
}

int isRelationBinaryOp(binType type) {
    return type == BIN_LESS_THAN || type == BIN_LESS_EQUAL ||
           type == BIN_GREATER_THAN || type == BIN_GREATER_EQUAL ||
           type == BIN_EQUALS || type == BIN_NOT_EQUALS;
}

CType* constantTypeToCType(constantType type) {
    switch (type) {
        case CONST_INT:           return C_CreateType(CTYPE_INT);
        case CONST_LONG:          return C_CreateType(CTYPE_LONG);
        case CONST_UNSIGNED_INT:  return C_CreateType(CTYPE_UINT);
        case CONST_UNSIGNED_LONG: return C_CreateType(CTYPE_ULONG);
        case CONST_FLOATING_POINT: return C_CreateType(CTYPE_DOUBLE);
        default:
            fprintf(stderr, "Invalid constant type in constantTypeToCType\n");
            exit(1);
    }
}

TACKYStaticVarType initialStaticTypeToTACKYStaticVarType(initialValueStaticInitType type) {
    switch (type) {
        case STATIC_INIT_INT:
        case STATIC_INIT_UNSIGNED_INT:
            return TACKY_INT;
        case STATIC_INIT_LONG:
        case STATIC_INIT_UNSIGNED_LONG:
            return TACKY_LONG;
        case STATIC_INIT_DOUBLE:
            return TACKY_DOUBLE;
        default:
            fprintf(stderr, "Invalid initial value static init type in initialStaticTypeToTACKYStaticVarType function\n");
            exit(1);
    }
}

int getIntegerConstant(TokenList* tokens) {
    constantType constType;
    uint64_t intValue;
    expectConstant(tokens, &constType, &intValue, NULL);
    if (constType == CONST_FLOATING_POINT) {
        fprintf(stderr, "Parser Error: Array size cannot be a floating point constant\n");
        exit(1);
    }
    return (int)intValue;
}