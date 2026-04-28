#pragma once
#include "../DataStructures/Map/Wrappers/CharIntMap.h"
#include "../Parser/AST/ASM-AST-Nodes/ASM-ASTNodes.h"
#include "../Parser/AST/ASM-AST-Nodes/ASM-ASTNodesUtilities/ASM-ASTNodesFree.h"


void pseudoToStackPositions(ASMInstructionList* instList, CharIntMap* table);
void insertPseudoToTable(CharIntMap* table, char* identifier, int *offset);
ASMOperand* changePseudoToStackOp(int offset, ASMOperand* operandToFree);
ASMOperand* handlePseudoOp(ASMOperand* operand, CharIntMap* table, int* offset);
void handleMemoryToMemoryForMov(ASMInstruction* inst);
void handleMemoryToMemoryForBinary(ASMInstruction* inst);
void handleMemoryToMemoryForCmp(ASMInstruction* inst);
void handleShiftingOperation(ASMInstruction* inst);
void handleImmediateAsDestForShift(ASMInstruction* inst);
void handleConstantAsDestIdivOperation(ASMInstruction* inst);
void handleConstantAsDestCmpOperation(ASMInstruction* inst);
void handleImulOperation(ASMInstruction* inst);
int fixStackSizeForFunction(int stackSize);