#include "ASMInstructionsGeneralUtils.h"
#include "../../Common/SharedTypeRank.h"

const int argResigters[] = {DI, SI, DX, CX, R8, R9};

int isRelationalOp(binType type) {
    return type == BIN_LESS_THAN || type == BIN_LESS_EQUAL || 
           type == BIN_GREATER_THAN || type == BIN_GREATER_EQUAL || 
           type == BIN_EQUALS || type == BIN_NOT_EQUALS;
}

ASMCondCode getCondCodeForRelationalOp(binType type, int isSigned) {
    switch(type) {
        case BIN_EQUALS: return ASM_COND_CODE_E;
        case BIN_NOT_EQUALS: return ASM_COND_CODE_NE;
        case BIN_LESS_THAN: return isSigned ? ASM_COND_CODE_L : ASM_COND_CODE_B;
        case BIN_LESS_EQUAL: return isSigned ? ASM_COND_CODE_LE : ASM_COND_CODE_BE;
        case BIN_GREATER_THAN: return isSigned ? ASM_COND_CODE_G : ASM_COND_CODE_A;
        case BIN_GREATER_EQUAL: return isSigned ? ASM_COND_CODE_GE : ASM_COND_CODE_AE;
        default: return -1; // Invalid relational operator
    }
}


ASMType convertTACKYTypeToASMType(TACKYValue* val, SymbolTable* symTable) {
    if (!val) return ASM_LONGWORD;

    if (val->type == TACKY_CONSTANT && val->constant) {
        if (val->constant->type == CONST_INT || val->constant->type == CONST_UNSIGNED_INT)
            return ASM_LONGWORD;
        if (val->constant->type == CONST_LONG || val->constant->type == CONST_UNSIGNED_LONG)
            return ASM_QUADWORD;
    }

    if ((val->type == TACKY_VAR || val->type == TACKY_STATIC) && val->identifier && symTable) {
        IdentifierTypeInfo* info = symbolTableLookup(symTable, val->identifier);
        if (info) return convertIdentifierTypeToASMType(info);
    }

    return ASM_LONGWORD;
}

ASMType convertSpecifierTypeToASMType(specifierType type) {
    switch (type) {
        case SPEC_INT:
        case SPEC_UNSIGNED_INT:
             return ASM_LONGWORD;
        case SPEC_LONG: 
        case SPEC_UNSIGNED_LONG:
             return ASM_QUADWORD;
        default: return ASM_LONGWORD; // Default to longword for other types
    }
}

ASMType convertIdentifierTypeToASMType(IdentifierTypeInfo* info) {
    if (!info) return ASM_LONGWORD;
    switch (info->type) {
        case TYPE_INT:
        case TYPE_UNSIGNED_INT:
            return ASM_LONGWORD;
        case TYPE_LONG:
        case TYPE_UNSIGNED_LONG:
            return ASM_QUADWORD;
        default: return ASM_LONGWORD;
    }
}

int signedOrUnsigned(TACKYInstruction* instruction, SymbolTable* symTable) {
    if (!instruction || instruction->type != TACKY_BINARY) return 1; // Default to signed

    TACKYValue* src1 = instruction->instValue.binaryOp.src1;
    TACKYValue* src2 = instruction->instValue.binaryOp.src2;
    TACKYValue* dest = instruction->instValue.binaryOp.dest;
    if (isSignedTACKYValue(src1, symTable) || isSignedTACKYValue(src2, symTable) || isSignedTACKYValue(dest, symTable)) return 1;
    return 0;
}


int isSignedTACKYValue(TACKYValue* val, SymbolTable* symTable) {
    if (!val) return 0; // Default to unsigned for null values

    if (val->type == TACKY_CONSTANT && val->constant) {
        return val->constant->type == CONST_INT || val->constant->type == CONST_LONG;
    }

    if ((val->type == TACKY_VAR || val->type == TACKY_STATIC) && val->identifier) {
        IdentifierTypeInfo* info = symbolTableLookup(symTable, val->identifier);
        if (info) {
            return isSignedIdentifierType(info->type);
        }
    }

    return 0; // Default to unsigned for other cases
}