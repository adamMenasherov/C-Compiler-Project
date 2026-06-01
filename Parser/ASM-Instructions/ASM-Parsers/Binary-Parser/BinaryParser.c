#include "BinaryParser.h"
#include "../../../Common/SharedTypeRank.h"

typedef ASMInstruction* (*divisionInst)(ASMOperand*, ASMType);

static divisionInst getDivisionInst(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    TACKYValue* src1 = instruction->instValue.binaryOp.src1;
    if (isSignedTACKYValue(src1, symTable)) {
        ASMInstruction* cdq_inst = createASMCDQInstruction(
            convertTACKYTypeToASMType(instruction->instValue.binaryOp.src1, symTable));
        addASMInstructionAtEnd(asmInstructionList, cdq_inst);
        return createIdivInstruction;
    } else {
        ASMInstruction* movInst = createMovInstruction(
            convertTACKYTypeToASMType(instruction->instValue.binaryOp.src1, symTable),
            createImmediateOperand(0),
            createRegisterOperand(DX)
        );
        addASMInstructionAtEnd(asmInstructionList, movInst);
        return createDivInstruction;
    }
}

static void createASMDoubleDivision(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    ASMOperand* src1 = tackyValueToASMOperand(instruction->instValue.binaryOp.src1, symTable);
    ASMOperand* src2 = tackyValueToASMOperand(instruction->instValue.binaryOp.src2, symTable);
    ASMOperand* dest = tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable);
    
    addASMInstructionAtEnd(asmInstructionList, 
        createMovInstruction(ASM_DOUBLE, src1, dest));
    addASMInstructionAtEnd(asmInstructionList,
        createASMBinaryInstruction(ASM_BINARY_DIVDOUBLE, ASM_DOUBLE, src2, dest));
}

static void handleDivideModuloCase(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    if (isDoubleOperand(instruction->instValue.binaryOp.src1, symTable) || isDoubleOperand(instruction->instValue.binaryOp.src2, symTable)) {
        createASMDoubleDivision(instruction, asmInstructionList, symTable);
        return;
    }

    ASMType src1_type = convertTACKYTypeToASMType(instruction->instValue.binaryOp.src1, symTable);
    ASMType dest_type = convertTACKYTypeToASMType(instruction->instValue.binaryOp.dest, symTable);
    ASMOperand* src1_op = tackyValueToASMOperand(instruction->instValue.binaryOp.src1, symTable);
    ASMOperand* src2_op = tackyValueToASMOperand(instruction->instValue.binaryOp.src2, symTable);
    ASMOperand* dest_op = tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable);

    addASMInstructionAtEnd(asmInstructionList, createMovInstruction(src1_type, src1_op, createRegisterOperand(AX)));
    divisionInst divInstFunc = getDivisionInst(instruction, asmInstructionList, symTable);
    addASMInstructionAtEnd(asmInstructionList, divInstFunc(src2_op, src1_type));

    if (instruction->instValue.binaryOp.binaryOpType == BIN_REMAINDER) {
        addASMInstructionAtEnd(asmInstructionList, createMovInstruction(dest_type, createRegisterOperand(DX), dest_op));
    } else {
        addASMInstructionAtEnd(asmInstructionList, createMovInstruction(dest_type, createRegisterOperand(AX), dest_op));
    }
}

static void handleBinaryRelationalOp(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    TACKYValue* src1 = instruction->instValue.binaryOp.src1;
    TACKYValue* src2 = instruction->instValue.binaryOp.src2;
    TACKYValue* dest = instruction->instValue.binaryOp.dest;
    ASMType src1_type = convertTACKYTypeToASMType(src1, symTable);
    ASMCondCode cond = getCondCodeForRelationalOp(instruction->instValue.binaryOp.binaryOpType,
                    signedOrUnsigned(instruction, symTable));

    addASMInstructionAtEnd(asmInstructionList,
        createASMCmpInstruction(src1_type,
            tackyValueToASMOperand(src2, symTable),
            tackyValueToASMOperand(src1, symTable)));
    addASMInstructionAtEnd(asmInstructionList,
        createMovInstruction(ASM_LONGWORD, createImmediateOperand(0), tackyValueToASMOperand(dest, symTable)));
    addASMInstructionAtEnd(asmInstructionList,
        createASMSetCCInstruction(cond, tackyValueToASMOperand(dest, symTable)));
}

void parseASMBinaryInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable) {
    if (instruction->instValue.binaryOp.binaryOpType == BIN_DIVIDE
            || instruction->instValue.binaryOp.binaryOpType == BIN_REMAINDER) {
        handleDivideModuloCase(instruction, asmInstructionList, symTable);
        return;
    }
    if (isRelationBinaryOp(instruction->instValue.binaryOp.binaryOpType)) {
        handleBinaryRelationalOp(instruction, asmInstructionList, symTable);
        return;
    }

    ASMType src1_type = convertTACKYTypeToASMType(instruction->instValue.binaryOp.src1, symTable);
    ASMOperand* src1_op = tackyValueToASMOperand(instruction->instValue.binaryOp.src1, symTable);
    ASMOperand* src2_op = tackyValueToASMOperand(instruction->instValue.binaryOp.src2, symTable);
    ASMOperand* dest_op_mov = tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable);
    ASMOperand* dest_op_bin = tackyValueToASMOperand(instruction->instValue.binaryOp.dest, symTable);

    addASMInstructionAtEnd(asmInstructionList, createMovInstruction(src1_type, src1_op, dest_op_mov));
    addASMInstructionAtEnd(asmInstructionList,
        createASMBinaryInstruction(
            binTypeToASMBinaryOperator(instruction->instValue.binaryOp.binaryOpType),
            src1_type, src2_op, dest_op_bin));
}
