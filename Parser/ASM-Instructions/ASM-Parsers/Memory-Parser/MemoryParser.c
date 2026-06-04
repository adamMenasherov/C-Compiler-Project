#include "../ASMParserInclude.h"
#include "MemoryParser.h"

void parseLoadInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* src_ptr = tackyValueToASMOperand(inst->instValue.load.src_ptr, symTable);
    ASMOperand* dest = tackyValueToASMOperand(inst->instValue.load.dest, symTable);
    ASMType valType = convertTACKYTypeToASMType(inst->instValue.load.dest, symTable);
    ASMOperand* ptrReg = createRegisterOperand(R10);

    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(ASM_QUADWORD, src_ptr, ptrReg));
    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(valType, createMemoryOperand(ptrReg, 0), dest));
}
void parseStoreInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* dst_ptr = tackyValueToASMOperand(inst->instValue.store.dst_ptr, symTable);
    ASMOperand* src = tackyValueToASMOperand(inst->instValue.store.src, symTable);
    ASMType valType = convertTACKYTypeToASMType(inst->instValue.store.src, symTable);
    ASMOperand* ptrReg = createRegisterOperand(R11);

    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(ASM_QUADWORD, dst_ptr, ptrReg));
    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(valType, src, createMemoryOperand(ptrReg, 0)));
}
void parseGetAddressInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* src = tackyValueToASMOperand(inst->instValue.getAddress.src, symTable);
    ASMOperand* dest = tackyValueToASMOperand(inst->instValue.getAddress.dest, symTable);
    
    addASMInstructionAtEnd(asmInstructionList,
        createLeaInstruction(src, dest));
}