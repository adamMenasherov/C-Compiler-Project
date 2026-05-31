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
    ASMType src_type  = convertTACKYTypeToASMType(instruction->instValue.doubleIntCast.src, symTable);
    ASMOperand* src_op  = tackyValueToASMOperand(instruction->instValue.doubleIntCast.src, symTable);
    ASMOperand* dest_op = tackyValueToASMOperand(instruction->instValue.doubleIntCast.dest, symTable);
    ASMOperand* rax_op  = createRegisterOperand(AX);

    if (src_type == ASM_LONGWORD) {
        addASMInstructionAtEnd(asmInstructionList, createASMMovZeroExtendInstruction(src_op, rax_op));
        addASMInstructionAtEnd(asmInstructionList, createCvtsi2sdInstruction(ASM_QUADWORD, rax_op, dest_op));
        return;
    }

    char* label1 = generateElseLabel();
    char* label2 = generateEndLabel();
    ASMOperand* rdx_op = createRegisterOperand(DX);

    addASMInstructionAtEnd(asmInstructionList, createASMCmpInstruction(ASM_QUADWORD, createImmediateOperand(0), src_op));
    addASMInstructionAtEnd(asmInstructionList, createASMJumpCCInstruction(ASM_COND_CODE_L, label1));
    addASMInstructionAtEnd(asmInstructionList, createCvtsi2sdInstruction(ASM_QUADWORD, src_op, dest_op));
    addASMInstructionAtEnd(asmInstructionList, createASMJumpInstruction(label2));
    addASMInstructionAtEnd(asmInstructionList, createASMLabelInstruction(label1));
    addASMInstructionAtEnd(asmInstructionList, createMovInstruction(ASM_QUADWORD, src_op, rdx_op));
    addASMInstructionAtEnd(asmInstructionList, createMovInstruction(ASM_QUADWORD, rdx_op, rax_op));
    addASMInstructionAtEnd(asmInstructionList, createASMBinaryInstruction(ASM_BINARY_SHIFT_RIGHT, ASM_QUADWORD, createImmediateOperand(1), rax_op));
    addASMInstructionAtEnd(asmInstructionList, createASMBinaryInstruction(ASM_BINARY_AND,         ASM_QUADWORD, createImmediateOperand(1), rdx_op));
    addASMInstructionAtEnd(asmInstructionList, createASMBinaryInstruction(ASM_BINARY_OR,          ASM_QUADWORD, rdx_op, rax_op));
    addASMInstructionAtEnd(asmInstructionList, createCvtsi2sdInstruction(ASM_QUADWORD, rax_op, dest_op));
    addASMInstructionAtEnd(asmInstructionList, createASMBinaryInstruction(ASM_BINARY_ADD,         ASM_DOUBLE,   dest_op, dest_op));
    addASMInstructionAtEnd(asmInstructionList, createASMLabelInstruction(label2));
}

/*
 * unsigned int:  Cvttsd2si(Quadword, src, R11)  +  Mov(Longword, R11, dst)
 * unsigned long: branch on upper-bound — subtract 2^63, convert, add 2^63 back
 */
void parseDoubleToUintInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMType dest_type = convertTACKYTypeToASMType(instruction->instValue.doubleIntCast.dest, symTable);
    ASMOperand* src_op  = tackyValueToASMOperand(instruction->instValue.doubleIntCast.src, symTable);
    ASMOperand* dest_op = tackyValueToASMOperand(instruction->instValue.doubleIntCast.dest, symTable);
    ASMOperand* rax_op  = createRegisterOperand(AX);

    if (dest_type == ASM_LONGWORD) {
        addASMInstructionAtEnd(asmInstructionList, createCvttsd2siInstruction(ASM_QUADWORD, src_op, rax_op));
        addASMInstructionAtEnd(asmInstructionList, createMovInstruction(ASM_LONGWORD, rax_op, dest_op));
        return;
    }

    char* label1  = generateElseLabel();
    char* label2  = generateEndLabel();
    ASMOperand* upper_op = createAsmConstantOperand(9223372036854775808.0, symTable);
    ASMOperand* xmm_op   = createRegisterOperand(XMM1);

    addASMInstructionAtEnd(asmInstructionList, createASMCmpInstruction(ASM_DOUBLE, upper_op, src_op));
    addASMInstructionAtEnd(asmInstructionList, createASMJumpCCInstruction(ASM_COND_CODE_AE, label1));
    addASMInstructionAtEnd(asmInstructionList, createCvttsd2siInstruction(ASM_QUADWORD, src_op, dest_op));
    addASMInstructionAtEnd(asmInstructionList, createASMJumpInstruction(label2));
    addASMInstructionAtEnd(asmInstructionList, createASMLabelInstruction(label1));
    addASMInstructionAtEnd(asmInstructionList, createMovInstruction(ASM_DOUBLE, src_op, xmm_op));
    addASMInstructionAtEnd(asmInstructionList, createASMBinaryInstruction(ASM_BINARY_SUBTRACT, ASM_DOUBLE, upper_op, xmm_op));
    addASMInstructionAtEnd(asmInstructionList, createCvttsd2siInstruction(ASM_QUADWORD, xmm_op, dest_op));
    addASMInstructionAtEnd(asmInstructionList, createMovInstruction(ASM_QUADWORD, createImmediateOperand((uint64_t)1 << 63), rax_op));
    addASMInstructionAtEnd(asmInstructionList, createASMBinaryInstruction(ASM_BINARY_ADD, ASM_QUADWORD, rax_op, dest_op));
    addASMInstructionAtEnd(asmInstructionList, createASMLabelInstruction(label2));
}
