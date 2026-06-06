#pragma once
#include "../../ASMInstructions.h"

void parseLoadInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable);
void parseStoreInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable);
void parseGetAddressInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable);
void parseAddPtrInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable);
void parseCopyToOffsetInstruction(TACKYInstruction* inst, ASMInstructionList* asmInstructionList, SymbolTable* symTable);