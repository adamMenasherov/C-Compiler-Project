#include "ASM-ASTNodesGeneralUtils.h"

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
