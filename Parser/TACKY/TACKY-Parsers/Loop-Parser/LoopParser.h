#pragma once
#include "../../TACKY_AST.h"


/**
 * @brief Parses a C AST while loop statement and emits corresponding TACKY instructions, adding them to the instruction list.
 * 
 * @param loop The C AST while loop statement to parse
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseWhileLoopInstructions(CLoop* loop, TACKYInstructionList* instructionList);

/**
 *  @brief Parses a C AST do-while loop statement and emits corresponding TACKY instructions, adding them to the instruction list.
 * 
 * @param loop The C AST do-while loop statement to parse
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseDoWhileLoopInstructions(CLoop* loop, TACKYInstructionList* instructionList);

/**
 * @brief Parses a C AST for loop statement and emits corresponding TACKY instructions, adding them to the instruction list.
 * 
 * @param forLoop The C AST for loop statement to parse
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseForLoopInstructions(CForLoop* forLoop, TACKYInstructionList* instructionList);

/**
 * @brief Parses the initialization part of a C AST for loop statement and emits corresponding TACKY instructions, adding them to the instruction list.
 * 
 * @param init The C AST for loop initialization to parse
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseForLoopInitInstructions(CForInit* init, TACKYInstructionList* instructionList);