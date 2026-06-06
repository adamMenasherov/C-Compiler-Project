#include "ASMInstructionsGeneralUtils.h"
#include "../../Common/SharedTypeRank.h"
#include "ASMInstructionsConstructors.h"
#include "../../../DataStructures/Map/Wrappers/DoubleStringMap.h"

static void handleConstantEntry(double val, const char* label, void* userData);

const int argResigters[] = {DI, SI, DX, CX, R8, R9};
const int doubleArgRegisters[] = {XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7};

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

ASMType asmByteArray(int sz, int al) {
    ASMType t;
    t.kind = ASM_BYTE_ARRAY;
    t.byteArray.size = sz;
    t.byteArray.alignment = al;
    return t;
}

ASMType convertTACKYTypeToASMType(TACKYValue* val, SymbolTable* symTable) {
    ASMType longword = {ASM_LONGWORD};
    if (!val) return longword;

    if (val->type == TACKY_CONSTANT && val->constant) {
        if (val->constant->type == CONST_FLOATING_POINT)
            return (ASMType){ASM_DOUBLE};
        if (val->constant->type == CONST_INT || val->constant->type == CONST_UNSIGNED_INT)
            return longword;
        if (val->constant->type == CONST_LONG || val->constant->type == CONST_UNSIGNED_LONG)
            return (ASMType){ASM_QUADWORD};
    }

    if ((val->type == TACKY_VAR || val->type == TACKY_STATIC) && val->identifier && symTable) {
        IdentifierTypeInfo* info = symbolTableLookup(symTable, val->identifier);
        if (info) return convertIdentifierTypeToASMType(info);
    }

    return longword;
}

ASMType convertCTypeToASMType(CType* type) {
    ASMType longword = {ASM_LONGWORD};
    if (!type) return longword;
    switch (type->kind) {
        case CTYPE_INT:
        case CTYPE_UINT:
             return longword;
        case CTYPE_LONG:
        case CTYPE_ULONG:
        case CTYPE_POINTER:
             return (ASMType){ASM_QUADWORD};
        case CTYPE_DOUBLE:
             return (ASMType){ASM_DOUBLE};
        case CTYPE_ARRAY: {
            int total_size = size(type);
            int elem_size = size(type->array.elementType);
            int arr_align = (total_size >= 16) ? 16 : elem_size;
            return asmByteArray(total_size, arr_align);
        }
        default: return longword;
    }
}

ASMType convertIdentifierTypeToASMType(IdentifierTypeInfo* info) {
    ASMType longword = {.kind = ASM_LONGWORD};
    if (!info || !info->type) return longword;
    return convertCTypeToASMType(info->type);
}

int signedOrUnsigned(TACKYInstruction* instruction, SymbolTable* symTable) {
    if (!instruction || instruction->type != TACKY_BINARY) return 1; // Default to signed

    TACKYValue* src1 = instruction->instValue.binaryOp.src1;
    TACKYValue* src2 = instruction->instValue.binaryOp.src2;
    if (isDoubleOperand(src1, symTable) || isDoubleOperand(src2, symTable))
        return 0;  // comisd sets CF/ZF, not SF/OF — use unsigned (AE/BE/A/B) conditions
    if (isSignedTACKYValue(src1, symTable) || isSignedTACKYValue(src2, symTable))
        return 1;
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
            return isSignedType(info->type);
        }
    }

    return 0; // Default to unsigned for other cases
}

void addConstantsAsTopLevels(ASMProgram* asmProgram, DoubleStringMap* cache) {
    doubleStringMapForEach(cache, handleConstantEntry, asmProgram->topLevels);
}

static void handleConstantEntry(double val, const char* label, void* userData) {
    ASMTopLevelArray* topLevels = (ASMTopLevelArray*)userData;
    ASMStaticConst* asmConst = createAsmStaticConst((char*)label, 8, val);
    ASMTopLevel* topLevel = createTopLevel(ASM_TOP_LEVEL_STATIC_CONST, asmConst);
    ASMTopLevelArray_append(topLevels, topLevel);
}

int isDoubleOperand(TACKYValue* val, SymbolTable* symTable) {
    if (!val) return 0;

    if (val->type == TACKY_CONSTANT && val->constant) {
        return val->constant->type == CONST_FLOATING_POINT;
    }

    if ((val->type == TACKY_VAR || val->type == TACKY_STATIC) && val->identifier) {
        IdentifierTypeInfo* info = symbolTableLookup(symTable, val->identifier);
        if (info) {
            ASMType asmType = convertIdentifierTypeToASMType(info);
            return asmType.kind == ASM_DOUBLE;
        }
    }

    return 0;
}