#pragma once
#include "../../TACKYProgram.h"


/**
 * @brief Parses a C AST block item and emits corresponding TACKY instructions, adding them to the instruction list.
 * 
 * @param blockItem The C AST block item to parse (could be a declaration or statement)
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseBlockItemInstructions(CBlockItem* blockItem, TACKYInstructionList* instructionList);

/**
 * @brief Parses a C AST block and emits corresponding TACKY instructions for each block item, adding them to the instruction list.
 * 
 * @param block The C AST block to parse, containing a list of block items (declarations and statements)
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseBlock(CBlock* block, TACKYInstructionList* instructionList);