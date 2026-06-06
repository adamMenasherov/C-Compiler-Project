#include "../ASMParserInclude.h"
#include "MemoryParser.h"

void parseLoadInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* src_ptr = tackyValueToASMOperand(inst->instValue.load.src_ptr, symTable);
    ASMOperand* dest = tackyValueToASMOperand(inst->instValue.load.dest, symTable);
    ASMType valType = convertTACKYTypeToASMType(inst->instValue.load.dest, symTable);
    ASMOperand* ptrReg = createRegisterOperand(R10);

    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction((ASMType){.kind = ASM_QUADWORD}, src_ptr, ptrReg));
    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(valType, createMemoryOperand(ptrReg, 0), dest));
}

void parseStoreInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* dst_ptr = tackyValueToASMOperand(inst->instValue.store.dst_ptr, symTable);
    ASMOperand* src = tackyValueToASMOperand(inst->instValue.store.src, symTable);
    ASMType valType = convertTACKYTypeToASMType(inst->instValue.store.src, symTable);
    ASMOperand* ptrReg = createRegisterOperand(R11);

    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction((ASMType){.kind = ASM_QUADWORD}, dst_ptr, ptrReg));
    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(valType, src, createMemoryOperand(ptrReg, 0)));
}

void parseGetAddressInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* src = tackyValueToASMOperand(inst->instValue.getAddress.src, symTable);
    ASMOperand* dest = tackyValueToASMOperand(inst->instValue.getAddress.dest, symTable);
    
    addASMInstructionAtEnd(asmInstructionList,
        createLeaInstruction(src, dest));
}

void parseCopyToOffsetInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* src = tackyValueToASMOperand(inst->instValue.copyToOffset.src, symTable);
    char* destIdentifier = inst->instValue.copyToOffset.dest->identifier;
    int offset = inst->instValue.copyToOffset.offset;
    ASMType valType = convertTACKYTypeToASMType(inst->instValue.copyToOffset.src, symTable);

    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(valType, src, createPsuedoMemoryOperand(destIdentifier, offset)));
}

void parseAddPtrInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* src_ptr = tackyValueToASMOperand(inst->instValue.addPtr.src_ptr, symTable);
    ASMOperand* index = tackyValueToASMOperand(inst->instValue.addPtr.index, symTable);
    int scale = inst->instValue.addPtr.scale;
    ASMOperand* dest = tackyValueToASMOperand(inst->instValue.addPtr.dest, symTable);
    ASMType valType = convertTACKYTypeToASMType(inst->instValue.addPtr.dest, symTable);
    ASMOperand* ptrReg = createRegisterOperand(AX);
    ASMOperand* indexReg = createRegisterOperand(DX);

    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction((ASMType){.kind = ASM_QUADWORD}, src_ptr, ptrReg));
    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction((ASMType){.kind = ASM_QUADWORD}, index, indexReg));
    if (scale != 1 && scale != 2 && scale != 4 && scale != 8) {
        addASMInstructionAtEnd(asmInstructionList,
            createASMBinaryInstruction(ASM_BINARY_MULTIPLY, (ASMType){.kind = ASM_QUADWORD}, createImmediateOperand(scale), indexReg));
            scale = 1;
    }
    addASMInstructionAtEnd(asmInstructionList,
        createLeaInstruction(createIndexedMemoryOperand(ptrReg, indexReg, scale), dest));
}