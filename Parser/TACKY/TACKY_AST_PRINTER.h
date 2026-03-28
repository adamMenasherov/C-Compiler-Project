#pragma once
#include "TACKY_AST.h"

/**
 * Prints a TACKYValue in the form:
 *   Constant(3)   or   Var("tmp.0")
 */
void printTACKYValue(const TACKYValue* val);

/**
 * Prints a single TACKYInstruction.
 * Unary ops print as:
 *   Unary(Negate, Constant(8), Var("tmp.0"))
 */
void printTACKYInstruction(const TACKYInstruction* inst);

/**
 * Prints every instruction in the list, one per line.
 */
void printTACKYInstructionList(const TACKYInstructionList* list);

/**
 * Prints a TACKYReturn as:
 *   Return(Var("tmp.0"))   or   Return(Constant(3))
 */
void printTACKYReturn(const TACKYReturn* ret);

/**
 * Prints a full TACKYFunction:
 *   -- function: <name> --
 *   <instructions>
 *   <return>
 */
void printTACKYFunction(const TACKYFunction* func);

/**
 * Prints the entire TACKYProgram.
 */
void printTACKYProgram(const TACKYProgram* program);
