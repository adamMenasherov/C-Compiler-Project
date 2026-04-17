#pragma once
#include "ASM-ASTNodesUtilities/ASM-ASTNodesGeneralUtils.h"
#include "ASM-ASTNodesUtilities/ASM-ASTNodesConstructors.h"



/**
 * Converts a TACKYProgram into its ASM representation.
 * @param tacky_prog The TACKY program to convert
 * @return A pointer to the generated ASMProgram
 */
ASMProgram* parseASMprogram(TACKYProgram* tacky_prog);

/**
 * Converts a TACKYFunction into its ASM representation, including
 * all instructions and the return sequence.
 * @param tacky_func The TACKY function to convert
 * @return A pointer to the generated ASMFunction
 */
ASMFunction* parseASMfunction(TACKYFunction* tacky_func);

/**
 * @brief Adds the ASM instructions to move the arguments from the caller locations to stack positions
 * 
 * @param instruction The TACKY function call instruction containing the arguments to be moved
 * @param asmInstructionList The ASM instruction list to which the generated instructions will be appended
 */
void addArgsAsInstructionsToFunc(TACKYInstructionList* instruction, ASMInstructionList* asmInstructionList);

/**
 * @brief Parses a TACKY return instruction and emits the corresponding ASM instructions to move the return value into the AX register and perform the return.
 * 
 * @param tacky_ret The TACKYValue representing the return value to be returned
 * @param instruction_list The ASM instruction list to which the generated instructions will be appended
 */
void parseASMReturn(TACKYValue* tacky_ret, ASMInstructionList* instruction_list);

/**
 * Converts a list of TACKY instructions into ASM instructions,
 * appending each result to the given ASM instruction list.
 * @param tackyInstList The source TACKY instruction list
 * @param asmInstructionList The destination ASM instruction list
 */
void parseASMInstruction(TACKYInstructionList* tackyInstList, ASMInstructionList* asmInstructionList);


/**
 * @brief Parses a TACKY function call instruction and emits the corresponding ASM instructions to perform the function call, including argument setup and handling of the return value.
 * 
 * @param instruction The TACKY instruction representing the function call
 * @param asmInstructionList The ASM instruction list to which the generated instructions will be appended
 */
void parseFunctionCallInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList);


/**
 * @brief Handles the translation of a TACKY unary instruction into its corresponding ASM instructions,
 * 
 * @param tackyInstList The TACKY instruction to process 
 * @param asmInstructionList The list of ASM instructions to append the generated instructions to
 */
void parseASMUnaryInstruction(TACKYInstruction* tackyInstList, ASMInstructionList* asmInstructionList);


/**
 * @brief Handles the translation of a TACKY binary instruction into its corresponding ASM instructions, including special handling for division and relational operations.
 * 
 * @param tackyInstList The TACKY instruction to process
 * @param asmInstructionList The list of ASM instructions to append the generated instructions to
 */
void parseASMBinaryInstruction(TACKYInstruction* tackyInstList, ASMInstructionList* asmInstructionList);

/**
 * @brief Handles the translation of a TACKY conditional jump instruction into its corresponding ASM instructions, including the necessary comparison and jump logic.
 * 
 * @param instruction The TACKY instruction representing the conditional jump
 * @param asmInstructionList The list of ASM instructions to append the generated instructions to
 */
void parseCondJumpInstruction(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList);


/* ============================================================
 * Instruction Case Handlers
 * ============================================================ */

/**
 * @brief Handles the special case for divide and modulo operations.
 * 
 * @param instruction The TACKY instruction for the divide or modulo operation
 * @param asmInstructionList The ASM instruction list to append to
 */
void handleDivideModuloCase(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList);

/**
 * @brief Handles the translation of a TACKY binary relational instruction into its corresponding ASM instructions.
 * 
 * @param instruction The TACKY instruction for the binary relational operation
 * @param asmInstructionList The ASM instruction list to append to
 */
void handleBinaryRelationalOp(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList);