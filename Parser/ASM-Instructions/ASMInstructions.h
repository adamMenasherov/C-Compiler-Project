#pragma once
#include "ASMInstructionsUtilities/ASMInstructionsGeneralUtils.h"
#include "ASMInstructionsUtilities/ASMInstructionsConstructors.h"

/**
 * Converts a TACKYProgram into its ASM representation.
 */
ASMProgram* parseASMprogram(TACKYProgram* tacky_prog, SymbolTable* symTable);

/**
 * Converts a TACKYFunction into its ASM representation, including
 * all instructions and the return sequence.
 */
ASMFunction* parseASMfunction(TACKYFunction* tacky_func, SymbolTable* symTable);

/**
 * Parses a TACKY return instruction and emits the corresponding ASM
 * instructions to move the return value into AX and perform the return.
 */
void parseASMReturn(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList, SymbolTable* symTable);
