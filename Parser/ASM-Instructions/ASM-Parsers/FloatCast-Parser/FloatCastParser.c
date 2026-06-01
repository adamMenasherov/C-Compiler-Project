#include "FloatCastParser.h"
#include "Parser/generateUtils.h"

/* Cvtsi2sd(<src type>, src, dst) */
void parseIntToDoubleInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMType src_type  = convertTACKYTypeToASMType(instruction->instValue.doubleIntCast.src, symTable);
    ASMOperand* src_op  = tackyValueToASMOperand(instruction->instValue.doubleIntCast.src, symTable);
    ASMOperand* dest_op = tackyValueToASMOperand(instruction->instValue.doubleIntCast.dest, symTable);
    addASMInstructionAtEnd(asmInstructionList, createCvtsi2sdInstruction(src_type, src_op, dest_op));
}

/* Cvttsd2si(<dst type>, src, dst) */
void parseDoubleToIntInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMType dest_type = convertTACKYTypeToASMType(instruction->instValue.doubleIntCast.dest, symTable);
    ASMOperand* src_op  = tackyValueToASMOperand(instruction->instValue.doubleIntCast.src, symTable);
    ASMOperand* dest_op = tackyValueToASMOperand(instruction->instValue.doubleIntCast.dest, symTable);
    addASMInstructionAtEnd(asmInstructionList, createCvttsd2siInstruction(dest_type, src_op, dest_op));
}

/*
 * unsigned int:  MovZeroExtend(src, R11)  +  Cvtsi2sd(Quadword, R11, dst)
 * unsigned long: branch on sign bit — halve, restore LSB, convert, double
 */
void parseUintToDoubleInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    TACKYValue* src_val  = instruction->instValue.doubleIntCast.src;
    TACKYValue* dest_val = instruction->instValue.doubleIntCast.dest;
    ASMType src_type = convertTACKYTypeToASMType(src_val, symTable);

    if (src_type == ASM_LONGWORD) {
        addASMInstructionAtEnd(asmInstructionList,
            createASMMovZeroExtendInstruction(tackyValueToASMOperand(src_val, symTable), createRegisterOperand(AX)));
        addASMInstructionAtEnd(asmInstructionList,
            createCvtsi2sdInstruction(ASM_QUADWORD, createRegisterOperand(AX), tackyValueToASMOperand(dest_val, symTable)));
        return;
    }

    char* label1 = generateElseLabel();
    char* label2 = generateEndLabel();

    addASMInstructionAtEnd(asmInstructionList,
        createASMCmpInstruction(ASM_QUADWORD, createImmediateOperand(0), tackyValueToASMOperand(src_val, symTable)));
    addASMInstructionAtEnd(asmInstructionList, createASMJumpCCInstruction(ASM_COND_CODE_L, label1));
    addASMInstructionAtEnd(asmInstructionList,
        createCvtsi2sdInstruction(ASM_QUADWORD, tackyValueToASMOperand(src_val, symTable), tackyValueToASMOperand(dest_val, symTable)));
    addASMInstructionAtEnd(asmInstructionList, createASMJumpInstruction(label2));
    addASMInstructionAtEnd(asmInstructionList, createASMLabelInstruction(label1));
    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(ASM_QUADWORD, tackyValueToASMOperand(src_val, symTable), createRegisterOperand(DX)));
    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(ASM_QUADWORD, createRegisterOperand(DX), createRegisterOperand(AX)));
    addASMInstructionAtEnd(asmInstructionList,
        createASMBinaryInstruction(ASM_BINARY_SHIFT_RIGHT, ASM_QUADWORD, createImmediateOperand(1), createRegisterOperand(AX)));
    addASMInstructionAtEnd(asmInstructionList,
        createASMBinaryInstruction(ASM_BINARY_AND, ASM_QUADWORD, createImmediateOperand(1), createRegisterOperand(DX)));
    addASMInstructionAtEnd(asmInstructionList,
        createASMBinaryInstruction(ASM_BINARY_OR, ASM_QUADWORD, createRegisterOperand(DX), createRegisterOperand(AX)));
    addASMInstructionAtEnd(asmInstructionList,
        createCvtsi2sdInstruction(ASM_QUADWORD, createRegisterOperand(AX), tackyValueToASMOperand(dest_val, symTable)));
    addASMInstructionAtEnd(asmInstructionList,
        createASMBinaryInstruction(ASM_BINARY_ADD, ASM_DOUBLE, tackyValueToASMOperand(dest_val, symTable), tackyValueToASMOperand(dest_val, symTable)));
    addASMInstructionAtEnd(asmInstructionList, createASMLabelInstruction(label2));
}

/*
 * unsigned int:  Cvttsd2si(Quadword, src, R11)  +  Mov(Longword, R11, dst)
 * unsigned long: branch on upper-bound — subtract 2^63, convert, add 2^63 back
 */
void parseDoubleToUintInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    TACKYValue* src_val  = instruction->instValue.doubleIntCast.src;
    TACKYValue* dest_val = instruction->instValue.doubleIntCast.dest;
    ASMType dest_type = convertTACKYTypeToASMType(dest_val, symTable);

    if (dest_type == ASM_LONGWORD) {
        addASMInstructionAtEnd(asmInstructionList,
            createCvttsd2siInstruction(ASM_QUADWORD, tackyValueToASMOperand(src_val, symTable), createRegisterOperand(AX)));
        addASMInstructionAtEnd(asmInstructionList,
            createMovInstruction(ASM_LONGWORD, createRegisterOperand(AX), tackyValueToASMOperand(dest_val, symTable)));
        return;
    }

    char* label1 = generateElseLabel();
    char* label2 = generateEndLabel();

    addASMInstructionAtEnd(asmInstructionList,
        createASMCmpInstruction(ASM_DOUBLE,
            createAsmConstantOperand(9223372036854775808.0, symTable),
            tackyValueToASMOperand(src_val, symTable)));
    addASMInstructionAtEnd(asmInstructionList, createASMJumpCCInstruction(ASM_COND_CODE_AE, label1));
    addASMInstructionAtEnd(asmInstructionList,
        createCvttsd2siInstruction(ASM_QUADWORD,
            tackyValueToASMOperand(src_val, symTable),
            tackyValueToASMOperand(dest_val, symTable)));
    addASMInstructionAtEnd(asmInstructionList, createASMJumpInstruction(label2));
    addASMInstructionAtEnd(asmInstructionList, createASMLabelInstruction(label1));
    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(ASM_DOUBLE, tackyValueToASMOperand(src_val, symTable), createRegisterOperand(XMM1)));
    addASMInstructionAtEnd(asmInstructionList,
        createASMBinaryInstruction(ASM_BINARY_SUBTRACT, ASM_DOUBLE,
            createAsmConstantOperand(9223372036854775808.0, symTable),
            createRegisterOperand(XMM1)));
    addASMInstructionAtEnd(asmInstructionList,
        createCvttsd2siInstruction(ASM_QUADWORD,
            createRegisterOperand(XMM1),
            tackyValueToASMOperand(dest_val, symTable)));
    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(ASM_QUADWORD, createImmediateOperand((uint64_t)1 << 63), createRegisterOperand(AX)));
    addASMInstructionAtEnd(asmInstructionList,
        createASMBinaryInstruction(ASM_BINARY_ADD, ASM_QUADWORD,
            createRegisterOperand(AX),
            tackyValueToASMOperand(dest_val, symTable)));
    addASMInstructionAtEnd(asmInstructionList, createASMLabelInstruction(label2));
}
