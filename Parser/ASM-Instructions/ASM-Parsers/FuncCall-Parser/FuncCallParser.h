#pragma once
#include "../../ASMInstructions.h"

typedef struct {
    int intRegIdxs[6];
    int intRegCount;
    int doubleRegIdxs[8];
    int doubleRegCount;
    int* stackIdxs;
    int stackCount;
} CallArgClassification;


void parseFunctionCallInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable);
