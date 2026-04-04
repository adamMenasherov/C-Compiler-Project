#pragma once
#include "../TACKY_AST.h"

/**
 * Frees a TACKYProgram and all of its owned resources recursively.
 * @param prog The program to free
 */
void freeTackyProgram(TACKYProgram* prog);

/**
 * Frees a TACKYFunction, including its instruction list and return node.
 * @param func The function to free
 */
void freeTackyFunction(TACKYFunction* func);

/**
 * Frees a TACKYInstructionList and all instructions it contains.
 * @param list The instruction list to free
 */
void freeTackyInstructionList(TACKYInstructionList* list);

/**
 * Frees a single TACKYInstruction and all of its operands.
 * @param instruction The instruction to free
 */
void freeTackyInstruction(TACKYInstruction* instruction);

/**
 * Frees a TACKYValue, including any owned constant or identifier string.
 * @param value The value to free
 */
void freeTackyValue(TACKYValue* value);

/**
 * Frees a TACKYConstant node.
 * @param constant The constant to free
 */
void freeTackyConstant(TACKYConstant* constant);