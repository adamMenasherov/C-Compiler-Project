#include "CastParser.h"

void parseASMSignExtendInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* src_op  = tackyValueToASMOperand(instruction->instValue.signExtend.src, symTable);
    ASMOperand* dest_op = tackyValueToASMOperand(instruction->instValue.signExtend.dest, symTable);
    addASMInstructionAtEnd(asmInstructionList, createASMMovsxInstruction(src_op, dest_op));
}

void parseZeroExtendInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* src_op  = tackyValueToASMOperand(instruction->instValue.zeroExtend.src, symTable);
    ASMOperand* dest_op = tackyValueToASMOperand(instruction->instValue.zeroExtend.dest, symTable);
    addASMInstructionAtEnd(asmInstructionList, createASMMovZeroExtendInstruction(src_op, dest_op));
}

void parseASMTruncateInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* src_op  = tackyValueToASMOperand(instruction->instValue.truncate.src, symTable);
    ASMOperand* dest_op = tackyValueToASMOperand(instruction->instValue.truncate.dest, symTable);
    addASMInstructionAtEnd(asmInstructionList, createMovInstruction(ASM_LONGWORD, src_op, dest_op));
}
