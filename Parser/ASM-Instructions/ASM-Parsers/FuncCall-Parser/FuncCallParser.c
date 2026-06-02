#include "FuncCallParser.h"
#include "Parser/TACKY/TACKYUtils/TACKYConstructors.h"
#include <stdlib.h>


static CallArgClassification classifyCallArgs(TACKYValueArray* args, SymbolTable* symTable) {
    CallArgClassification cls = {0};
    int n = TACKYValueArray_size(args);
    cls.stackIdxs = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        TACKYValue* arg = TACKYValueArray_get(args, i);
        if (isDoubleOperand(arg, symTable)) {
            if (cls.doubleRegCount < 8)
                cls.doubleRegIdxs[cls.doubleRegCount++] = i;
            else
                cls.stackIdxs[cls.stackCount++] = i;
        } else {
            if (cls.intRegCount < 6)
                cls.intRegIdxs[cls.intRegCount++] = i;
            else
                cls.stackIdxs[cls.stackCount++] = i;
        }
    }
    cls.stackIdxs = realloc(cls.stackIdxs, cls.stackCount * sizeof(int));
    return cls;
}

static void emitIntRegArgs(TACKYValueArray* args, CallArgClassification* cls,
                           ASMInstructionList* list, SymbolTable* symTable) {
    for (int i = 0; i < cls->intRegCount; i++) {
        TACKYValue* arg = TACKYValueArray_get(args, cls->intRegIdxs[i]);
        ASMType argType = convertTACKYTypeToASMType(arg, symTable);
        addASMInstructionAtEnd(list, createMovInstruction(argType,
            tackyValueToASMOperand(arg, symTable),
            createRegisterOperand(argResigters[i])));
    }
}

static void emitDoubleRegArgs(TACKYValueArray* args, CallArgClassification* cls,
                              ASMInstructionList* list, SymbolTable* symTable) {
    for (int i = 0; i < cls->doubleRegCount; i++) {
        TACKYValue* arg = TACKYValueArray_get(args, cls->doubleRegIdxs[i]);
        addASMInstructionAtEnd(list, createMovInstruction(ASM_DOUBLE,
            tackyValueToASMOperand(arg, symTable),
            createRegisterOperand(doubleArgRegisters[i])));
    }
}

static void emitStackArgs(TACKYValueArray* args, CallArgClassification* cls,
                          ASMInstructionList* list, SymbolTable* symTable) {
    for (int j = cls->stackCount - 1; j >= 0; j--) {
        TACKYValue* arg = TACKYValueArray_get(args, cls->stackIdxs[j]);
        ASMOperand* argOp = tackyValueToASMOperand(arg, symTable);
        ASMType argType = isDoubleOperand(arg, symTable)
            ? ASM_DOUBLE
            : convertTACKYTypeToASMType(arg, symTable);
        if (argOp->type == ASM_OP_IMMEDIATE || argOp->type == ASM_OP_REGISTER
                || argType == ASM_QUADWORD || argType == ASM_DOUBLE) {
            addASMInstructionAtEnd(list, createASMPushInstruction(argOp));
        } else {
            addASMInstructionAtEnd(list, createMovInstruction(argType, argOp, createRegisterOperand(AX)));
            addASMInstructionAtEnd(list, createASMPushInstruction(createRegisterOperand(AX)));
        }
    }
}

void parseFunctionCallInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    TACKYValueArray* args = instruction->instValue.funCall.args;
    CallArgClassification cls = classifyCallArgs(args, symTable);

    int stackPadding = (cls.stackCount % 2) ? 8 : 0;
    if (stackPadding)
        addASMInstructionAtEnd(asmInstructionList, createASMAllocateStackInstruction(stackPadding));

    emitIntRegArgs(args, &cls, asmInstructionList, symTable);
    emitDoubleRegArgs(args, &cls, asmInstructionList, symTable);
    emitStackArgs(args, &cls, asmInstructionList, symTable);

    addASMInstructionAtEnd(asmInstructionList,
        createASMCallInstruction(instruction->instValue.funCall.functionName));

    int bytesToRemove = 8 * cls.stackCount + stackPadding;
    if (bytesToRemove)
        addASMInstructionAtEnd(asmInstructionList, createASMDeallocateStackInstruction(bytesToRemove));

    ASMType retAsmType = ASM_LONGWORD;
    IdentifierTypeInfo* calledFunction = symbolTableLookup(symTable, instruction->instValue.funCall.functionName);
    if (calledFunction && calledFunction->type && calledFunction->type->kind == CTYPE_FUN && calledFunction->type->fun.ret) {
        retAsmType = convertCTypeToASMType(calledFunction->type->fun.ret);
    }
    ASMOperand* retDest = tackyValueToASMOperand(instruction->instValue.funCall.resultVar, symTable);
    Register retReg = (retAsmType == ASM_DOUBLE) ? XMM0 : AX;
    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(retAsmType, createRegisterOperand(retReg), retDest));
    free(cls.stackIdxs);
}
