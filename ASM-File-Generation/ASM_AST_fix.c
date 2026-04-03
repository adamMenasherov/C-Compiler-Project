#include "ASM_AST_fix.h"
#include "../DataStructures/HashTable.h"
#include "../Parser/AST/ASM-AST-Nodes/ASM-ASTNodesUtilities/ASM-ASTNodesFree.h"
#include <stdlib.h>

void pseudoToStackPositions(ASMInstructionList* instList, HashTable* table) {
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
    
    int storedOffset;
    ht_get(table, operand->OperandValue.identifier, &storedOffset);
    return changePseudoToStackOp(storedOffset, operand);
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

void handleImulOperation(ASMInstruction* inst) {
    if (inst->type != ASM_BINARY || inst->instValue.binary.type != ASM_BINARY_MULTIPLY) return;
    if (inst->instValue.binary.op2->type != ASM_OP_STACK) return;

    ASMOperand* memOp2 = inst->instValue.binary.op2;
    ASMInstruction* mov1 = createMovInstruction(memOp2, createRegisterOperand(R11));
    ASMInstruction* mov2 = createMovInstruction(createRegisterOperand(R11), memOp2);
    inst->instValue.binary.op2 = createRegisterOperand(R11);

    if (mov1) {
        ASMInstruction* iMulInst = calloc(1, sizeof(ASMInstruction));
        *iMulInst = *inst;

        mov2->next = inst->next;
        iMulInst->next = mov2;

        *inst = *mov1;
        inst->next = iMulInst;
        free(mov1);
    }
}