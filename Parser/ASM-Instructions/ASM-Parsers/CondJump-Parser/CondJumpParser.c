#include "CondJumpParser.h"

static void handleDoubleCondJump(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable, 
    ASMCondCode cond, ASMOperand* cond_op) {
    ASMOperand* xmm0 = createRegisterOperand(XMM0);

    addASMInstructionAtEnd(asmInstructionList, 
        createASMBinaryInstruction(ASM_BINARY_XOR, ASM_DOUBLE, xmm0 , xmm0)); // zeroing out xmm0
    addASMInstructionAtEnd(asmInstructionList,
        createASMCmpInstruction(ASM_DOUBLE, cond_op, xmm0));
    addASMInstructionAtEnd(asmInstructionList, 
        createASMJumpCCInstruction(cond, instruction->instValue.condJump.label));
}


void parseCondJumpInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMCondCode cond = (instruction->type == TACKY_JUMP_IF_ZERO) ? ASM_COND_CODE_E : ASM_COND_CODE_NE;
    ASMType cond_type  = convertTACKYTypeToASMType(instruction->instValue.condJump.condition, symTable);
    ASMOperand* cond_op = tackyValueToASMOperand(instruction->instValue.condJump.condition, symTable);
    if (cond_type == ASM_DOUBLE) {
        handleDoubleCondJump(instruction, asmInstructionList, symTable, cond, cond_op);
        return;
    }

    addASMInstructionAtEnd(asmInstructionList, createASMCmpInstruction(cond_type, createImmediateOperand(0), cond_op));
    addASMInstructionAtEnd(asmInstructionList, createASMJumpCCInstruction(cond, instruction->instValue.condJump.label));
}
