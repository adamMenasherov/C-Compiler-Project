#include "UnaryParser.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/TokenExpect/C-ASTNodeExpect.h"

static void handleUnaryNot(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    TACKYValue* src  = instruction->instValue.unaryOp.src;
    TACKYValue* dest = instruction->instValue.unaryOp.dest;
    ASMType src_type = convertTACKYTypeToASMType(src, symTable);

    ASMOperand* cmp_zero = (src_type == ASM_DOUBLE)
        ? createAsmConstantOperand(0.0, symTable)
        : createImmediateOperand(0);

    addASMInstructionAtEnd(asmInstructionList,
        createASMCmpInstruction(src_type, cmp_zero, tackyValueToASMOperand(src, symTable)));
    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(ASM_LONGWORD, createImmediateOperand(0), tackyValueToASMOperand(dest, symTable)));
    addASMInstructionAtEnd(asmInstructionList,
        createASMSetCCInstruction(ASM_COND_CODE_E, tackyValueToASMOperand(dest, symTable)));
}

static void handleDoubleNegation(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    TACKYValue* src  = instruction->instValue.unaryOp.src;
    TACKYValue* dest = instruction->instValue.unaryOp.dest;

    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(ASM_DOUBLE,
            tackyValueToASMOperand(src, symTable),
            tackyValueToASMOperand(dest, symTable)));
    addASMInstructionAtEnd(asmInstructionList,
        createASMBinaryInstruction(ASM_BINARY_XOR, ASM_DOUBLE,
            createAsmConstantOperand(-0.0, symTable),
            tackyValueToASMOperand(dest, symTable)));
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
