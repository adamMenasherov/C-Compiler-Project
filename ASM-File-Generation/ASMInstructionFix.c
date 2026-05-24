#include "ASMInstructionFix.h"
#include "../Parser/ASM-Instructions/ASMInstructionsUtilities/ASMInstructionsFree.h"
#include "../DataStructures/HashTable/Wrappers/AsmSymbolTableWrapper.h"
#include <stdlib.h>

static int getPseudoSlotSize(const char* identifier, ASMSymbolTable* asmSymTable) {
    ASMSymTabEntry* entry;
    if (!identifier || !asmSymTable) return 4;

    entry = asmSymbolTableLookup(asmSymTable, identifier);
    if (!entry || entry->entryType != ASM_SYMTAB_OBJ_ENTRY) return 4;
    return entry->objEntry.assemblyType == ASM_QUADWORD ? 8 : 4;
}

void pseudoToStackPositions(ASMInstructionList* instList, CharIntMap* table, ASMSymbolTable* asmSymTable) {
    int usedStackBytes = 0;
    ASMInstruction* inst;
    ASMOperand* newOp;
    for (inst = instList->head; inst != NULL; inst = inst->next) {
        switch (inst->type) {
            case ASM_MOV:
            {
                if ((newOp = handlePseudoOp(inst->instValue.mov.operand1, table, &usedStackBytes, asmSymTable))) {
                    inst->instValue.mov.operand1 = newOp;
                }
                if ((newOp = handlePseudoOp(inst->instValue.mov.operand2, table, &usedStackBytes, asmSymTable))) {
                    inst->instValue.mov.operand2 = newOp;
                }
                handleMemoryToMemoryForMov(inst);
                break;
            }
            case ASM_MOVSX:
            {
                if ((newOp = handlePseudoOp(inst->instValue.movsx.operand1, table, &usedStackBytes, asmSymTable))) {
                    inst->instValue.movsx.operand1 = newOp;
                }
                if ((newOp = handlePseudoOp(inst->instValue.movsx.operand2, table, &usedStackBytes, asmSymTable))) {
                    inst->instValue.movsx.operand2 = newOp;
                }
                handleMovsxInstruction(inst);
                break;
            }
            case ASM_IDIV:
            {
                if ((newOp = handlePseudoOp(inst->instValue.idiv.divisor, table, &usedStackBytes, asmSymTable))) {
                    inst->instValue.idiv.divisor = newOp;
                }
                handleConstantAsDestIdivOperation(inst);
                break;
            }
            case ASM_UNARY:
            {
                if ((newOp = handlePseudoOp(inst->instValue.unary.op, table, &usedStackBytes, asmSymTable))) {
                    inst->instValue.unary.op = newOp;
                }
                break;
            }
            case ASM_BINARY:
            {
                if ((newOp = handlePseudoOp(inst->instValue.binary.op1, table, &usedStackBytes, asmSymTable))) {
                    inst->instValue.binary.op1 = newOp;
                }
                if ((newOp = handlePseudoOp(inst->instValue.binary.op2, table, &usedStackBytes, asmSymTable))) {
                    inst->instValue.binary.op2 = newOp;
                }
                handleShiftingOperation(inst);
                handleMemoryToMemoryForBinary(inst);
                handleImulOperation(inst);
                break;
            }
            case ASM_CMP:
            {
                if ((newOp = handlePseudoOp(inst->instValue.cmp.op1, table, &usedStackBytes, asmSymTable))) {
                    inst->instValue.cmp.op1 = newOp;
                }
                if ((newOp = handlePseudoOp(inst->instValue.cmp.op2, table, &usedStackBytes, asmSymTable))) {
                    inst->instValue.cmp.op2 = newOp;
                }
                handleMemoryToMemoryForCmp(inst);
                handleConstantAsDestCmpOperation(inst);
                break;
            }
            case ASM_SETCC:
            {
                if ((newOp = handlePseudoOp(inst->instValue.setcc.op, table, &usedStackBytes, asmSymTable))) {
                    inst->instValue.setcc.op = newOp;
                }
                break;
            }
            default: continue;
        }
    }

    charIntMapPrint(table);
    addASMInstructionAtBeginning(instList, createAllocStackInstruction(fixStackSizeForFunction(usedStackBytes)));
}


void insertPseudoToTable(CharIntMap* table, char* identifier, int* offset, ASMSymbolTable* asmSymTable) {
    int val;
    int size;
    int alignment;
    int used;
    if (charIntMapGet(table, identifier, &val)) return;

    size = getPseudoSlotSize(identifier, asmSymTable);
    alignment = size;
    used = *offset + size;
    if (used % alignment != 0) {
        used += alignment - (used % alignment);
    }

    charIntMapPut(table, identifier, -used);
    *offset = used;
}


ASMOperand* changePseudoToStackOp(int offset, ASMOperand* operandToFree) {
    ASMOperand* stckOp = createStackOperand(offset);
    freeASMOperand(operandToFree);
    return stckOp;
}


ASMOperand* handlePseudoOp(ASMOperand* operand, CharIntMap* table, int* offset, ASMSymbolTable* asmSymTable) {
    if (!operand) return NULL;
    if (operand->type != ASM_OP_PSEUDO) return NULL;
    insertPseudoToTable(table, operand->OperandValue.identifier, offset, asmSymTable);

    int storedOffset;
    charIntMapGet(table, operand->OperandValue.identifier, &storedOffset);
    return changePseudoToStackOp(storedOffset, operand);
}

static int isMemoryOp(ASMOperand* op) {
    return op && (op->type == ASM_OP_STACK || op->type == ASM_OP_DATA);
}

static int isBothMemoryOps(ASMOperand* op1, ASMOperand* op2) {
    return isMemoryOp(op1) && isMemoryOp(op2);
}


void handleMemoryToMemoryForMov(ASMInstruction* inst) {
    if (!isBothMemoryOps(inst->instValue.mov.operand1, inst->instValue.mov.operand2)) return;
    ASMOperand* dst = inst->instValue.mov.operand2;
    inst->instValue.mov.operand2 = createRegisterOperand(R10);

    ASMInstruction* movToDest = createMovInstruction(inst->instValue.mov.asmType, createRegisterOperand(R10), dst);
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
    ASMInstruction* movToCx = createMovInstruction(ASM_LONGWORD, countOp, createRegisterOperand(CX));
    ASMInstruction* shiftInst = calloc(1, sizeof(ASMInstruction));
    ASMInstruction* popRcx = createASMPopInstruction(createRegisterOperand(CX));
    if (!pushRcx || !movToCx || !shiftInst || !popRcx) return;

    *shiftInst = *inst;
    shiftInst->instValue.binary.op1 = createRegisterOperand(CX);

    ASMInstruction* movDestToR11 = NULL;
    if (shiftInst->instValue.binary.op2->type == ASM_OP_IMMEDIATE) {
        ASMOperand* immDest = shiftInst->instValue.binary.op2;
        shiftInst->instValue.binary.op2 = createRegisterOperand(R11);
        movDestToR11 = createMovInstruction(shiftInst->instValue.binary.asmType, immDest, createRegisterOperand(R11));
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

void handleMemoryToMemoryForBinary(ASMInstruction* inst) {
    if (!isBothMemoryOps(inst->instValue.binary.op1, inst->instValue.binary.op2)) return;
    ASMOperand* op1 = inst->instValue.binary.op1;
    inst->instValue.binary.op1 = createRegisterOperand(R10);

    ASMInstruction* movToR10 = createMovInstruction(inst->instValue.binary.asmType, op1, createRegisterOperand(R10));
    if (movToR10) {
        ASMInstruction* binaryInst = calloc(1, sizeof(ASMInstruction));
        *binaryInst = *inst;
        binaryInst->next = inst->next;

        *inst = *movToR10;
        inst->next = binaryInst;
        free(movToR10);
    }
}

void handleMemoryToMemoryForCmp(ASMInstruction* inst) {
    if (!isBothMemoryOps(inst->instValue.cmp.op1, inst->instValue.cmp.op2)) return;
    ASMOperand* op1 = inst->instValue.cmp.op1;
    inst->instValue.cmp.op1 = createRegisterOperand(R10);

    ASMInstruction* movToR10 = createMovInstruction(inst->instValue.cmp.asmType, op1, createRegisterOperand(R10));
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

    ASMInstruction* movToR10 = createMovInstruction(inst->instValue.idiv.asmType, divisor, createRegisterOperand(R10));
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

    ASMInstruction* movToR11 = createMovInstruction(inst->instValue.cmp.asmType, op2, createRegisterOperand(R11));
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

    ASMInstruction* movToR11 = createMovInstruction(inst->instValue.binary.asmType, immOp, createRegisterOperand(R11));
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
    ASMInstruction* mov1 = createMovInstruction(inst->instValue.binary.asmType, memOp2, createRegisterOperand(R11));
    ASMInstruction* mov2 = createMovInstruction(inst->instValue.binary.asmType, createRegisterOperand(R11), memOp2Copy);
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


void handleMovsxInstruction(ASMInstruction* inst) {
    if (inst->type != ASM_MOVSX) return;

    ASMOperand* src = inst->instValue.movsx.operand1;
    ASMOperand* dst = inst->instValue.movsx.operand2;
    int needsSrcRewrite = src && src->type == ASM_OP_IMMEDIATE;
    int needsDstRewrite = isMemoryOp(dst);
    ASMInstruction* originalNext = inst->next;

    if (!needsSrcRewrite && !needsDstRewrite) return;

    ASMInstruction* first = NULL;
    ASMInstruction* tail = NULL;

    if (needsSrcRewrite) {
        ASMInstruction* movSrcToR10 = createMovInstruction(ASM_LONGWORD, src, createRegisterOperand(R10));
        if (!movSrcToR10) return;
        first = movSrcToR10;
        tail = movSrcToR10;
    }

    ASMInstruction* movsxInst = calloc(1, sizeof(ASMInstruction));
    if (!movsxInst) return;
    movsxInst->type = ASM_MOVSX;
    movsxInst->instValue.movsx.operand1 = needsSrcRewrite ? createRegisterOperand(R10) : src;
    movsxInst->instValue.movsx.operand2 = needsDstRewrite ? createRegisterOperand(R11) : dst;

    if (!first) {
        first = movsxInst;
        tail = movsxInst;
    } else {
        tail->next = movsxInst;
        tail = movsxInst;
    }

    if (needsDstRewrite) {
        ASMInstruction* movR11ToDest = createMovInstruction(ASM_QUADWORD, createRegisterOperand(R11), dst);
        if (!movR11ToDest) return;
        tail->next = movR11ToDest;
        tail = movR11ToDest;
    }
    tail->next = originalNext;
    *inst = *first;
    free(first);
}

int fixStackSizeForFunction(int stackSize) {
    if (stackSize <= 0) return 0;
    int rem = stackSize % 16;
    if (rem == 0) return stackSize;
    return stackSize + (16 - rem);
}