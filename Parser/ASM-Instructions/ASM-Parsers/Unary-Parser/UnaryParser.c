#include "UnaryParser.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/TokenExpect/C-ASTNodeExpect.h"

static void handleUnaryNot(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMType src_type  = convertTACKYTypeToASMType(instruction->instValue.unaryOp.src, symTable);
    ASMType dest_type = convertTACKYTypeToASMType(instruction->instValue.unaryOp.dest, symTable);
    ASMOperand* src_op  = tackyValueToASMOperand(instruction->instValue.unaryOp.src, symTable);
    ASMOperand* dest_op = tackyValueToASMOperand(instruction->instValue.unaryOp.dest, symTable);

    addASMInstructionAtEnd(asmInstructionList, createASMCmpInstruction(src_type, createImmediateOperand(0), src_op));
    addASMInstructionAtEnd(asmInstructionList, createMovInstruction(dest_type, createImmediateOperand(0), dest_op));
    addASMInstructionAtEnd(asmInstructionList, createASMSetCCInstruction(ASM_COND_CODE_E, dest_op));
}

static void handleDoubleNegation(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* src_op  = tackyValueToASMOperand(instruction->instValue.unaryOp.src, symTable);
    ASMOperand* dest_op = tackyValueToASMOperand(instruction->instValue.unaryOp.dest, symTable);
    ASMOperand* zero_op = createAsmConstantOperand(-0.0, symTable);

    addASMInstructionAtEnd(asmInstructionList, 
        createMovInstruction(ASM_DOUBLE, src_op, dest_op));
    addASMInstructionAtEnd(asmInstructionList,
        createASMBinaryInstruction(ASM_BINARY_XOR, ASM_DOUBLE, zero_op, dest_op));
}


void parseASMUnaryInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    if (instruction->type != TACKY_UNARY) return;
    if (instruction->instValue.unaryOp.type == UNARY_NOT) {
        handleUnaryNot(instruction, asmInstructionList, symTable);
        return;
    }

    TACKYValue* src  = instruction->instValue.unaryOp.src;
    TACKYValue* dest = instruction->instValue.unaryOp.dest;
    ASMType src_type  = convertTACKYTypeToASMType(src, symTable);
    ASMType dest_type = convertTACKYTypeToASMType(dest, symTable);
    ASMOperand* src_op  = tackyValueToASMOperand(src, symTable);
    ASMOperand* dest_op = tackyValueToASMOperand(dest, symTable);
    unaryType unType = instruction->instValue.unaryOp.type;

    if (unType == UNARY_NEGATE && src_type == ASM_DOUBLE) {
        handleDoubleNegation(instruction, asmInstructionList, symTable);
        return;
    }
    if (isIncrementDecrementOp(instruction->instValue.unaryOp.type)) {
        addASMInstructionAtEnd(asmInstructionList,
            createASMUnaryInstruction(instruction->instValue.unaryOp.type, src_type, src, symTable));
        addASMInstructionAtEnd(asmInstructionList,
            createMovInstruction(src_type, src_op, dest_op));
    } else {
        addASMInstructionAtEnd(asmInstructionList,
            createMovInstruction(src_type, src_op, dest_op));
        addASMInstructionAtEnd(asmInstructionList,
            createASMUnaryInstruction(instruction->instValue.unaryOp.type, dest_type, dest, symTable));
    }
}
