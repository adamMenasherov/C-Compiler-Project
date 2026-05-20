#pragma once
#include "../TACKYProgram.h"

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

/**
 * Prints a TACKYTopLevel, which may be a static variable or a function.  
 */
void printTACKYTopLevel(const TACKYTopLevel* topLevel);

/**
 * Prints a TACKYStaticVar. 
 */
void printTACKYStaticVar(const TACKYStaticVar* staticVar);
