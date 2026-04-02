#include "ASM_AST_fix.h"
#include "../DataStructures/HashTable.h"
#include "../Parser/AST/ASM-AST-Nodes/ASM-ASTNodesUtilities/ASM-ASTNodesFree.h"
#include <stdlib.h>

void pseudoToStackPositions(ASMInstructionList* instList, HashTable* table) {
    static int offset = -4;
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
                if ((newOp = handlePseudoOp(inst->instValue.mov.operand1, table, &offset))) {
                    inst->instValue.mov.operand1 = newOp;
                }
                handleIdivOperation(inst);
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
                handleStackToStackForBinary(inst);
                break;
            }
            default: continue;
        }
    }
    ht_print(table);
    addASMInstructionAtBeginning(instList, createAllocStackInstruction(-(offset + 4)));
}


void insertPseudoToTable(HashTable* table, char* identifier, int* offset) {
    int val;
    if (ht_get(table, identifier, &val)) return;
    ht_insert(table, identifier, *offset);
    *offset -= 4;
}


ASMOperand* changePseudoToStackOp(int offset, ASMOperand* operandToFree) {
    ASMOperand* stckOp = createStackOperand(offset);
    freeASMOperand(operandToFree);
    return stckOp;
}


ASMOperand* handlePseudoOp(ASMOperand* operand, HashTable* table, int* offset) {
    if (operand->type != ASM_OP_PSEUDO) return NULL;
    insertPseudoToTable(table, operand->OperandValue.identifier, offset);
    return changePseudoToStackOp(*offset + 4, operand);
}

int isBothStackOps(ASMOperand* op1, ASMOperand* op2) {
    return (op1 && op1->type == ASM_OP_STACK) && (op2 && op2->type == ASM_OP_STACK);
}


void handleStackToStackForMov(ASMInstruction* inst) {
    if (!isBothStackOps(inst->instValue.mov.operand1, inst->instValue.mov.operand2)) return;
    ASMOperand* dst = inst->instValue.mov.operand2;
    inst->instValue.mov.operand2 = createRegisterOperand(R10);

    // Insert Mov(R10, dst) after inst
    ASMInstruction* movToDest = createMovInstruction(createRegisterOperand(R10), dst);
    movToDest->next = inst->next;
    inst->next = movToDest;
}

void handleStackToStackForBinary(ASMInstruction* inst) {
    if (!isBothStackOps(inst->instValue.binary.op1, inst->instValue.binary.op2)) return;
    ASMOperand* op1 = inst->instValue.binary.op1;
    inst->instValue.binary.op1 = createRegisterOperand(R10);

    // Insert Mov(R10, op2) after inst
    ASMInstruction* movToR10 = createMovInstruction(op1 ,createRegisterOperand(R10));
    if (movToR10) {
        movToR10->next = inst;
        *inst = *movToR10; 
        free(movToR10); 
    }
}

void handleIdivOperation(ASMInstruction* inst) {
    if (inst->type != ASM_IDIV) return;
    if (inst->instValue.idiv.divisor->type != ASM_OP_IMMEDIATE) return;

    // Move dividend to AX
    ASMInstruction* movDividend = createMovInstruction(inst->instValue.mov.operand1, createRegisterOperand(R10));
    inst->instValue.idiv.divisor = inst->instValue.mov.operand2;

    // Insert movDividend before the current instruction
    if (movDividend) {
        movDividend->next = inst;
        *inst = *movDividend; // Copy contents of movDividend into inst
        free(movDividend); // Free the temporary instruction
    }
}