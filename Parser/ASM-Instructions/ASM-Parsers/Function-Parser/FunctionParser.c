#include "FunctionParser.h"
#include <stdlib.h>
#include "../Unary-Parser/UnaryParser.h"
#include "../Binary-Parser/BinaryParser.h"
#include "../CondJump-Parser/CondJumpParser.h"
#include "../FuncCall-Parser/FuncCallParser.h"
#include "../Cast-Parser/CastParser.h"
#include "../FloatCast-Parser/FloatCastParser.h"
#include "Parser/TACKY/TACKYUtils/TACKYConstructors.h"

typedef CallArgClassification ParamClassification;
typedef void (*ASMInstructionHandler)(TACKYInstruction*, ASMInstructionList*, SymbolTable*);

static void handleDispatchCopy(TACKYInstruction* inst, ASMInstructionList* list, SymbolTable* symTable) {
    ASMType src_type  = convertTACKYTypeToASMType(inst->instValue.copy.src, symTable);
    ASMOperand* src_op  = tackyValueToASMOperand(inst->instValue.copy.src, symTable);
    ASMOperand* dest_op = tackyValueToASMOperand(inst->instValue.copy.dest, symTable);
    addASMInstructionAtEnd(list, createMovInstruction(src_type, src_op, dest_op));
}

static void handleDispatchJump(TACKYInstruction* inst, ASMInstructionList* list, SymbolTable* symTable) {
    (void)symTable;
    addASMInstructionAtEnd(list, createASMJumpInstruction(inst->instValue.jump.label));
}

static void handleDispatchLabel(TACKYInstruction* inst, ASMInstructionList* list, SymbolTable* symTable) {
    (void)symTable;
    addASMInstructionAtEnd(list, createASMLabelInstruction(inst->instValue.label.label));
}

static const ASMInstructionHandler instructionHandlers[] = {
    [TACKY_UNARY]            = parseASMUnaryInstruction,
    [TACKY_BINARY]           = parseASMBinaryInstruction,
    [TACKY_COPY]             = handleDispatchCopy,
    [TACKY_JUMP]             = handleDispatchJump,
    [TACKY_JUMP_IF_ZERO]     = parseCondJumpInstruction,
    [TACKY_JUMP_IF_NOT_ZERO] = parseCondJumpInstruction,
    [TACKY_RETURN]           = parseASMReturn,
    [TACKY_FUNCTION_CALL]    = parseFunctionCallInstruction,
    [TACKY_LABEL]            = handleDispatchLabel,
    [TACKY_SIGN_EXTEND]      = parseASMSignExtendInstruction,
    [TACKY_ZERO_EXTEND]      = parseZeroExtendInstruction,
    [TACKY_TRUNCATE]         = parseASMTruncateInstruction,
    [TACKY_DOUBLE_TO_INT]    = parseDoubleToIntInstruction,
    [TACKY_DOUBLE_TO_UINT]   = parseDoubleToUintInstruction,
    [TACKY_INT_TO_DOUBLE]    = parseIntToDoubleInstruction,
    [TACKY_UINT_TO_DOUBLE]   = parseUintToDoubleInstruction,
};

void parseASMInstruction(TACKYInstructionList* tackyInstList, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    if (InstructionArray_getCursor(tackyInstList) >= InstructionArray_size(tackyInstList)) return;
    TACKYInstruction* instruction = InstructionArray_nextAndGet(tackyInstList);
    if (!instruction) return;
    if (instruction->type < (int)(sizeof(instructionHandlers) / sizeof(instructionHandlers[0]))
            && instructionHandlers[instruction->type]) {
        instructionHandlers[instruction->type](instruction, asmInstructionList, symTable);
    }
}


static ParamClassification classifyFuncParams(CType* funcType, int paramCount) {
    ParamClassification cls = {0};
    cls.stackIdxs = malloc(paramCount * sizeof(int));
    for (int i = 0; i < paramCount; i++) {
        ASMType t = ASM_LONGWORD;
        if (funcType && funcType->kind == CTYPE_FUN && i < funcType->fun.paramCnt && funcType->fun.params[i])
            t = convertCTypeToASMType(funcType->fun.params[i]);
        if (t == ASM_DOUBLE) {
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
    return cls;
}

static void emitIntRegParams(IdentifierArray* params, CType* funcType,
                             ParamClassification* cls, ASMInstructionList* list, SymbolTable* symTable) {
    for (int i = 0; i < cls->intRegCount; i++) {
        int idx = cls->intRegIdxs[i];
        char* arg = IdentifierArray_get(params, idx);
        ASMType paramType = ASM_LONGWORD;
        if (funcType && funcType->kind == CTYPE_FUN && idx < funcType->fun.paramCnt && funcType->fun.params[idx])
            paramType = convertCTypeToASMType(funcType->fun.params[idx]);
        addASMInstructionAtEnd(list,
            createMovInstruction(paramType,
                createRegisterOperand(argResigters[i]),
                tackyValueToASMOperand(createVarValue(arg), symTable)));
    }
}

static void emitDoubleRegParams(IdentifierArray* params, ParamClassification* cls,
                                ASMInstructionList* list, SymbolTable* symTable) {
    for (int i = 0; i < cls->doubleRegCount; i++) {
        int idx = cls->doubleRegIdxs[i];
        char* arg = IdentifierArray_get(params, idx);
        addASMInstructionAtEnd(list,
            createMovInstruction(ASM_DOUBLE,
                createRegisterOperand(doubleArgRegisters[i]),
                tackyValueToASMOperand(createVarValue(arg), symTable)));
    }
}

static void emitStackParams(IdentifierArray* params, CType* funcType,
                            ParamClassification* cls, ASMInstructionList* list, SymbolTable* symTable) {
    int stackPos = 16;
    for (int j = 0; j < cls->stackCount; j++) {
        int idx = cls->stackIdxs[j];
        char* arg = IdentifierArray_get(params, idx);
        ASMType paramType = ASM_LONGWORD;
        if (funcType && funcType->kind == CTYPE_FUN && idx < funcType->fun.paramCnt && funcType->fun.params[idx])
            paramType = convertCTypeToASMType(funcType->fun.params[idx]);
        addASMInstructionAtEnd(list,
            createMovInstruction(paramType,
                createStackOperand(stackPos),
                tackyValueToASMOperand(createVarValue(arg), symTable)));
        stackPos += 8;
    }
}

void addArgsAsInstructionsToFunc(TACKYFunction* tacky_func, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    IdentifierTypeInfo* function = symbolTableLookup(symTable, tacky_func->function_name);
    CType* funcType = function ? function->type : NULL;
    IdentifierArray* params = tacky_func->parameters;
    int lenArgs = IdentifierArray_size(params);

    ParamClassification cls = classifyFuncParams(funcType, lenArgs);
    emitIntRegParams(params, funcType, &cls, asmInstructionList, symTable);
    emitDoubleRegParams(params, &cls, asmInstructionList, symTable);
    emitStackParams(params, funcType, &cls, asmInstructionList, symTable);

    free(cls.stackIdxs);
}
