#include "ASM_AST_fix.h"
#include "../Parser/AST/ASM-AST-Nodes/ASM-ASTNodesUtilities/ASM-ASTNodesFree.h"
#include <stdlib.h>

void pseudoToStackPositions(ASMInstructionList* instList, CharIntMap* table) {
    int offset = -4;
    ASMInstruction* inst;
    ASMOperand* newOp;
    for (inst = instList->head; inst != NULL; inst = inst->next) {
        switch (inst->type) {
            case ASM_MOV:
            {
                if ((newOp = handlePseudoOp(inst->instValue.mov.operand1, table, &offset))) {
                    inst->instValue.mov.operand1 = newOp;
                }
                if ((newOp = handlePseudoOp(inst->instValue.mov.operand2, table, &offset))) {
                    inst->instValue.mov.operand2 = newOp;
                }
                handleStackToStackForMov(inst);
                break;
            }
            case ASM_IDIV:
            {
                if ((newOp = handlePseudoOp(inst->instValue.idiv.divisor, table, &offset))) {
                    inst->instValue.idiv.divisor = newOp;
                }
                handleConstantAsDestIdivOperation(inst);
                break;
            }
            case ASM_UNARY:
            {
                if ((newOp = handlePseudoOp(inst->instValue.unary.op, table, &offset))) {
                    inst->instValue.unary.op = newOp;
                }
                break;
            }
            case ASM_BINARY:
            {
                if ((newOp = handlePseudoOp(inst->instValue.binary.op1, table, &offset))) {
                    inst->instValue.binary.op1 = newOp;
                }
                if ((newOp = handlePseudoOp(inst->instValue.binary.op2, table, &offset))) {
                    inst->instValue.binary.op2 = newOp;
                }
                handleShiftingOperation(inst);
                handleStackToStackForBinary(inst);
                handleImulOperation(inst);
                break;
            }
            case ASM_CMP:
            {
                if ((newOp = handlePseudoOp(inst->instValue.cmp.op1, table, &offset))) {
                    inst->instValue.cmp.op1 = newOp;
                }
                if ((newOp = handlePseudoOp(inst->instValue.cmp.op2, table, &offset))) {
                    inst->instValue.cmp.op2 = newOp;
                }
                handleStackToStackForCmp(inst);
                handleConstantAsDestCmpOperation(inst);
                break;
            }
            case ASM_SETCC:
            {
                if ((newOp = handlePseudoOp(inst->instValue.setcc.op, table, &offset))) {
                    inst->instValue.setcc.op = newOp;
                }
                break;
            }
            default: continue;
        }
    }

    charIntMapPrint(table);
    addASMInstructionAtBeginning(instList, createAllocStackInstruction(fixStackSizeForFunction(-(offset + 4))));
}


void insertPseudoToTable(CharIntMap* table, char* identifier, int* offset) {
    int val;
    if (charIntMapGet(table, identifier, &val)) return;
    charIntMapPut(table, identifier, *offset);
    *offset -= 4;
}


ASMOperand* changePseudoToStackOp(int offset, ASMOperand* operandToFree) {
    ASMOperand* stckOp = createStackOperand(offset);
    freeASMOperand(operandToFree);
    return stckOp;
}


ASMOperand* handlePseudoOp(ASMOperand* operand, CharIntMap* table, int* offset) {
    if (!operand) return NULL;
    if (operand->type != ASM_OP_PSEUDO) return NULL;
    insertPseudoToTable(table, operand->OperandValue.identifier, offset);

    int storedOffset;
    charIntMapGet(table, operand->OperandValue.identifier, &storedOffset);
    return changePseudoToStackOp(storedOffset, operand);
}

int isBothStackOps(ASMOperand* op1, ASMOperand* op2) {
    return (op1 && op1->type == ASM_OP_STACK) && (op2 && op2->type == ASM_OP_STACK);
}


void handleStackToStackForMov(ASMInstruction* inst) {
    if (!isBothStackOps(inst->instValue.mov.operand1, inst->instValue.mov.operand2)) return;
    ASMOperand* dst = inst->instValue.mov.operand2;
    inst->instValue.mov.operand2 = createRegisterOperand(R10);

    ASMInstruction* movToDest = createMovInstruction(createRegisterOperand(R10), dst);
    movToDest->next = inst->next;
    inst->next = movToDest;
}

void handleShiftingOperation(ASMInstruction* inst) {
    if (inst->type != ASM_BINARY) return;
    if (inst->instValue.binary.type != ASM_BINARY_SHIFT_LEFT &&
        inst->instValue.binary.type != ASM_BINARY_SHIFT_RIGHT) return;
    if (!inst->instValue.binary.op1 || !inst->instValue.binary.op2) return;
    if (inst->instValue.binary.op1->type == ASM_OP_IMMEDIATE) return;
    if (inst->instValue.binary.op1->type == ASM_OP_REGISTER &&
        inst->instValue.binary.op1->OperandValue.reg == CX) return;

    ASMInstruction* originalNext = inst->next;
    ASMOperand* countOp = inst->instValue.binary.op1;

    ASMInstruction* pushRcx = createASMPushInstruction(createRegisterOperand(CX));
    ASMInstruction* movToCx = createMovInstruction(countOp, createRegisterOperand(CX));
    ASMInstruction* shiftInst = calloc(1, sizeof(ASMInstruction));
    ASMInstruction* popRcx = createASMPopInstruction(createRegisterOperand(CX));
    if (!pushRcx || !movToCx || !shiftInst || !popRcx) return;

    *shiftInst = *inst;
    shiftInst->instValue.binary.op1 = createRegisterOperand(CX);

    ASMInstruction* movDestToR11 = NULL;
    if (shiftInst->instValue.binary.op2->type == ASM_OP_IMMEDIATE) {
        ASMOperand* immDest = shiftInst->instValue.binary.op2;
        shiftInst->instValue.binary.op2 = createRegisterOperand(R11);
        movDestToR11 = createMovInstruction(immDest, createRegisterOperand(R11));
        if (!movDestToR11) return;
    }

    pushRcx->next = movToCx;
    if (movDestToR11) {
        movToCx->next = movDestToR11;
        movDestToR11->next = shiftInst;
    } else {
        movToCx->next = shiftInst;
    }
    shiftInst->next = popRcx;
    popRcx->next = originalNext;

    *inst = *pushRcx;
    inst->next = movToCx;
    free(pushRcx);
}

void handleStackToStackForBinary(ASMInstruction* inst) {
    if (!isBothStackOps(inst->instValue.binary.op1, inst->instValue.binary.op2)) return;
    ASMOperand* op1 = inst->instValue.binary.op1;
    inst->instValue.binary.op1 = createRegisterOperand(R10);

    ASMInstruction* movToR10 = createMovInstruction(op1, createRegisterOperand(R10));
    if (movToR10) {
        ASMInstruction* binaryInst = calloc(1, sizeof(ASMInstruction));
        *binaryInst = *inst;
        binaryInst->next = inst->next;

        *inst = *movToR10;
        inst->next = binaryInst;
        free(movToR10);
    }
}

void handleStackToStackForCmp(ASMInstruction* inst) {
    if (!isBothStackOps(inst->instValue.cmp.op1, inst->instValue.cmp.op2)) return;
    ASMOperand* op1 = inst->instValue.cmp.op1;
    inst->instValue.cmp.op1 = createRegisterOperand(R10);

    ASMInstruction* movToR10 = createMovInstruction(op1, createRegisterOperand(R10));
    if (movToR10) {
        ASMInstruction* cmpInst = calloc(1, sizeof(ASMInstruction));
        *cmpInst = *inst;
        cmpInst->next = inst->next;

        *inst = *movToR10;
        inst->next = cmpInst;
        free(movToR10);
    }
}

void handleConstantAsDestIdivOperation(ASMInstruction* inst) {
    if (inst->type != ASM_IDIV) return;
    if (!inst->instValue.idiv.divisor || inst->instValue.idiv.divisor->type == ASM_OP_REGISTER) {
        return;
    }

    ASMOperand* divisor = inst->instValue.idiv.divisor;
    inst->instValue.idiv.divisor = createRegisterOperand(R10);

    ASMInstruction* movToR10 = createMovInstruction(divisor, createRegisterOperand(R10));
    if (movToR10) {
        ASMInstruction* idivInst = calloc(1, sizeof(ASMInstruction));
        *idivInst = *inst;
        idivInst->next = inst->next;

        *inst = *movToR10;
        inst->next = idivInst;
        free(movToR10);
    }
}

void handleConstantAsDestCmpOperation(ASMInstruction* inst) {
    if (inst->type != ASM_CMP) return;
    if (!inst->instValue.cmp.op2 || inst->instValue.cmp.op2->type == ASM_OP_REGISTER) {
        return;
    }

    ASMOperand* op2 = inst->instValue.cmp.op2;
    inst->instValue.cmp.op2 = createRegisterOperand(R11);

    ASMInstruction* movToR11 = createMovInstruction(op2, createRegisterOperand(R11));
    if (movToR11) {
        ASMInstruction* cmpInst = calloc(1, sizeof(ASMInstruction));
        *cmpInst = *inst;
        cmpInst->next = inst->next;

        *inst = *movToR11;
        inst->next = cmpInst;
        free(movToR11);
    }
}


void handleImmediateAsDestForShift(ASMInstruction* inst) {
    if (inst->type != ASM_BINARY) return;
    if (inst->instValue.binary.type != ASM_BINARY_SHIFT_LEFT && inst->instValue.binary.type != ASM_BINARY_SHIFT_RIGHT) return;
    if (inst->instValue.binary.op2->type != ASM_OP_IMMEDIATE) return;

    ASMOperand* immOp = inst->instValue.binary.op2;
    inst->instValue.binary.op2 = createRegisterOperand(R11);

    ASMInstruction* movToR11 = createMovInstruction(immOp, createRegisterOperand(R11));
    if (movToR11) {
        ASMInstruction* shiftInst = calloc(1, sizeof(ASMInstruction));
        *shiftInst = *inst;
        shiftInst->next = inst->next;

        *inst = *movToR11;
        inst->next = shiftInst;
        free(movToR11);
    }
}

void handleImulOperation(ASMInstruction* inst) {
    if (inst->type != ASM_BINARY || inst->instValue.binary.type != ASM_BINARY_MULTIPLY) return;
    if (inst->instValue.binary.op2->type != ASM_OP_STACK) return;

    ASMOperand* memOp2 = inst->instValue.binary.op2;
    ASMOperand* memOp2Copy = createStackOperand(memOp2->OperandValue.immediate);
    ASMInstruction* mov1 = createMovInstruction(memOp2, createRegisterOperand(R11));
    ASMInstruction* mov2 = createMovInstruction(createRegisterOperand(R11), memOp2Copy);
    inst->instValue.binary.op2 = createRegisterOperand(R11);

    if (mov1 && mov2 && memOp2Copy) {
        ASMInstruction* iMulInst = calloc(1, sizeof(ASMInstruction));
        *iMulInst = *inst;

        mov2->next = inst->next;
        iMulInst->next = mov2;

        *inst = *mov1;
        inst->next = iMulInst;
        free(mov1);
        return;
    }

    freeASMOperand(memOp2Copy);
    freeASMInstruction(mov1);
    freeASMInstruction(mov2);
}

int fixStackSizeForFunction(int stackSize) {
    return stackSize + (16 - stackSize % 16);
}