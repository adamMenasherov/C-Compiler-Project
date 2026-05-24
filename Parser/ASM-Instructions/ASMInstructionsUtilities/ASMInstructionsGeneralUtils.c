#include "ASMInstructionsGeneralUtils.h"

const int argResigters[] = {DI, SI, DX, CX, R8, R9};

int isRelationalOp(binType type) {
    return type == BIN_LESS_THAN || type == BIN_LESS_EQUAL || 
           type == BIN_GREATER_THAN || type == BIN_GREATER_EQUAL || 
           type == BIN_EQUALS || type == BIN_NOT_EQUALS;
}

ASMCondCode getCondCodeForRelationalOp(binType type) {
    switch(type) {
        case BIN_EQUALS: return ASM_COND_CODE_E;
        case BIN_NOT_EQUALS: return ASM_COND_CODE_NE;
        case BIN_LESS_THAN: return ASM_COND_CODE_L;
        case BIN_LESS_EQUAL: return ASM_COND_CODE_LE;
        case BIN_GREATER_THAN: return ASM_COND_CODE_G;
        case BIN_GREATER_EQUAL: return ASM_COND_CODE_GE;
        default: return -1; // Invalid relational operator
    }
}


ASMType convertTACKYTypeToASMType(TACKYValue* val) {
    if (val->constant->type == CONST_INT) {
        return ASM_LONGWORD;
    } else if (val->constant->type == CONST_LONG) {
        return ASM_QUADWORD;
    }
    return ASM_LONGWORD; // Default to longword for variables (assuming 32-bit)
}

ASMType convertSpecifierTypeToASMType(specifierType type) {
    switch (type) {
        case SPEC_INT: return ASM_LONGWORD;
        case SPEC_LONG: return ASM_QUADWORD;
        default: return ASM_LONGWORD; // Default to longword for other types
    }
}

ASMType convertIdentifierTypeToASMType(IdentifierTypeInfo* info) {
    if (!info) return ASM_LONGWORD; // Default to longword for unknown types
    switch (info->type) {
        case TYPE_INT: return ASM_LONGWORD;
        case TYPE_LONG: return ASM_QUADWORD;
        default: return ASM_LONGWORD; // Default to longword for other types
    }
}