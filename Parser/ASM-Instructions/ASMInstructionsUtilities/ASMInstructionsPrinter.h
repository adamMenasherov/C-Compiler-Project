#pragma once
#include "../ASMInstructions.h"

/**
 * Prints an ASMOperand in the form:
 *   Imm(3)   |   Reg(AX)   |   Pseudo("tmp.0")
 */
void printASMOperand(const ASMOperand* operand);

/**
 * Prints a single ASMInstruction, e.g.:
 *   Mov(Imm(3), Reg(AX))
 *   Unary(Neg, Pseudo("tmp.0"))
 *   Ret
 */
void printASMInstruction(const ASMInstruction* inst);

/**
 * Prints every instruction in the list, one per line.
 */
void printASMInstructionList(const ASMInstructionList* list);

/**
 * Prints an ASMFunction in the form:
 *   Function(name,
 *       <instructions>
 *   )
 */
void printASMFunction(const ASMFunction* func);

/**
 * Prints the full ASMProgram in the form:
 *   Program(
 *       <function>
 *   )
 */
void printASMProgram(const ASMProgram* program);

