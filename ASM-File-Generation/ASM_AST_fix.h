#pragma once
#include "../DataStructures/HashTable.h"
#include "../Parser/AST/ASM-AST-Nodes/ASM-ASTNodes.h"
#include "../Parser/AST/ASM-AST-Nodes/ASM-ASTNodesUtilities/ASM-ASTNodesFree.h"


void pseudoToStackPositions(ASMInstructionList* instList, HashTable* table);
void insertPseudoToTable(HashTable* table, char* identifier, int *offset);
ASMOperand* changePseudoToStackOp(int offset, ASMOperand* operandToFree);
ASMOperand* handlePseudoOp(ASMOperand* operand, HashTable* table, int* offset);
int isBothStackOps(ASMOperand* op1, ASMOperand* op2);
void handleStackToStackForMov(ASMInstruction* inst);
void handleStackToStackForBinary(ASMInstruction* inst);
void handleStackToStackForCmp(ASMInstruction* inst);
void handleConstantAsDestIdivOperation(ASMInstruction* inst);
void handleConstantAsDestCmpOperation(ASMInstruction* inst);
void handleImulOperation(ASMInstruction* inst);