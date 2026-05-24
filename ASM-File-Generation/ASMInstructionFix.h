#pragma once
#include "../DataStructures/Map/Wrappers/CharIntMap.h"
#include "../Parser/ASM-Instructions/ASMInstructions.h"
#include "../Parser/ASM-Instructions/ASMInstructionsUtilities/ASMInstructionsFree.h"
#include "../DataStructures/HashTable/Wrappers/AsmSymbolTableWrapper.h"


void pseudoToStackPositions(ASMInstructionList* instList, CharIntMap* table, ASMSymbolTable* asmSymTable);
void insertPseudoToTable(CharIntMap* table, char* identifier, int *offset, ASMSymbolTable* asmSymTable);
ASMOperand* changePseudoToStackOp(int offset, ASMOperand* operandToFree);
ASMOperand* handlePseudoOp(ASMOperand* operand, CharIntMap* table, int* offset, ASMSymbolTable* asmSymTable);
void handleMemoryToMemoryForMov(ASMInstruction* inst);
void handleMemoryToMemoryForBinary(ASMInstruction* inst);
void handleMovsxInstruction(ASMInstruction* inst);
void handleMemoryToMemoryForCmp(ASMInstruction* inst);
void handleShiftingOperation(ASMInstruction* inst);
void handleImmediateAsDestForShift(ASMInstruction* inst);
void handleConstantAsDestIdivOperation(ASMInstruction* inst);
void handleConstantAsDestCmpOperation(ASMInstruction* inst);
void handleImulOperation(ASMInstruction* inst);
int fixStackSizeForFunction(int stackSize);