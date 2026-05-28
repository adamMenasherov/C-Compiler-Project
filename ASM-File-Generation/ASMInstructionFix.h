#pragma once
#include "../DataStructures/Map/Wrappers/CharIntMap.h"
#include "../Parser/ASM-Instructions/ASMInstructions.h"
#include "../Parser/ASM-Instructions/ASMInstructionsUtilities/ASMInstructionsFree.h"
#include "../DataStructures/HashTable/Wrappers/AsmSymbolTableWrapper.h"


void pseudoToStackPositions(ASMInstructionList* instList, CharIntMap* table,
                            ASMSymbolTable* asmSymTable, SymbolTable* symTable);
int fixStackSizeForFunction(int stackSize);