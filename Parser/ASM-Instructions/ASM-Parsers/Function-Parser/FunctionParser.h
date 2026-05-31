#pragma once
#include "../../ASMInstructions.h"

void addArgsAsInstructionsToFunc(TACKYFunction* tacky_func, ASMInstructionList* asmInstructionList, SymbolTable* symTable);
void parseASMInstruction(TACKYInstructionList* tackyInstList, ASMInstructionList* asmInstructionList, SymbolTable* symTable);
