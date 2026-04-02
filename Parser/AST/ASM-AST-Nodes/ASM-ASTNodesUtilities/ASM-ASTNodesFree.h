#pragma once
#include "../ASM-ASTNodes.h"

/**
 * Frees the memory allocated for an ASM operand.
 * @param operand The ASM operand to free
 */
void freeASMOperand(ASMOperand* operand);

/**
 * Frees the memory allocated for an ASM instruction.
 * @param inst The ASM instruction to free
 */
void freeASMInstruction(ASMInstruction* inst);

/**
 * Frees the memory allocated for an ASM instruction list and all its instructions.
 * @param list The ASM instruction list to free
 */
void freeASMInstructionList(ASMInstructionList* list);

/**
 * Frees the memory allocated for an ASM function and all its instructions.
 * @param func The ASM function to free
 */
void freeASMFunction(ASMFunction* func);

/**
 * Frees the memory allocated for an entire ASM program and all its functions.
 * @param program The ASM program to free
 */
void freeASMProgram(ASMProgram* program);