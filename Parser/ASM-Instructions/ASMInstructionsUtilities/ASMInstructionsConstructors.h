#pragma once
#include "ASMInstructionsGeneralUtils.h"

/* ============================================================
 * Instruction List Management
 * ============================================================ */

/**
 * Initializes and returns a new empty ASMInstructionList.
 * @return A pointer to the newly allocated instruction list
 */
ASMInstructionList* createASMInstructionList();

/**
 * Appends an ASMInstruction to the list, growing it if necessary.
 * @param list The instruction list to append to
 * @param instruction The instruction to append
 */
void addASMInstructionAtEnd(ASMInstructionList* list, ASMInstruction* instruction);

/**
 * @brief   Prepends an ASMInstruction to the list, adjusting head and tail pointers as needed.
 * @param   list The instruction list to prepend to
 * @param   instruction The instruction to prepend
 */
void addASMInstructionAtBeginning(ASMInstructionList* list, ASMInstruction* instruction);


/* ============================================================
 * Operand Creation
 * ============================================================ */

/**
 * @brief Create a Register Operand object
 * 
 * @param reg The register to create the operand for
 * @return ASMOperand* pointer to the created register operand
 */
ASMOperand* createRegisterOperand(Register reg);

/**
 * @brief Create a Immediate Operand object
 * 
 * @param value The immediate value to be stored in the operand
 * @return ASMOperand* pointer to the created immediate operand
 */
ASMOperand* createImmediateOperand(uint64_t value);

/**
 * @brief Create a Stack Operand object
 * 
 * @param offset The offset of the stack location
 * @return ASMOperand* pointer to the created stack operand
 */
ASMOperand* createStackOperand(int offset);

/**
 * @brief Converts a TACKYValue into an ASMOperand, handling both constants and variables.
 * 
 * @param val The TACKY value to convert
 * @return ASMOperand* pointer to the created ASMOperand
 */
ASMOperand* tackyValueToASMOperand(TACKYValue* val, SymbolTable* symTable);


/* ============================================================
 * Instruction Creation
 * ============================================================ */

/**
 * @brief Creates an ASMInstruction representing a unary operation (e.g. NEG, NOT).
 * 
 * @param type The type of unary operation
 * @param dest The destination operand where the result will be stored
 * @return A pointer to the created ASMInstruction
 */
ASMInstruction* createASMUnaryInstruction(unaryType type, ASMType asmType, TACKYValue* dest, SymbolTable* symTable);

/**
 * @brief Creates an ASMInstruction representing a binary operation (e.g. ADD, SUB, MUL).
 * 
 * @param type The type of binary operation
 * @param op1 The first operand for the binary operation
 * @param op2 The second operand for the binary operation
 * @return A pointer to the created ASMInstruction
 */
ASMBinaryOperator binTypeToASMBinaryOperator(binType type);
ASMInstruction* createASMBinaryInstruction(ASMBinaryOperator op, ASMType asmType, ASMOperand* op1, ASMOperand* op2);

/**
 * @brief Create a Mov Instruction object
 * 
 * @param src The source operand for the MOV instruction
 * @param dest The destination operand for the MOV instruction
 * @return ASMInstruction* pointer to the created MOV instruction 
 */
ASMInstruction* createMovInstruction(ASMType asmType, ASMOperand* src, ASMOperand* dest);

/**
 * @brief Create a Alloc Stack Instruction object
 * 
 * @param size The size of the stack to allocate (in bytes)
 * @return ASMInstruction* pointer to the created Alloc Stack instruction
 */
ASMInstruction* createAllocStackInstruction(int size);

/**
 * @brief Create a Idiv Instruction object
 * 
 * @param divisor The operand representing the divisor for the IDIV instruction
 * @return ASMInstruction* pointer to the created IDIV instruction
 */
ASMInstruction* createIdivInstruction(ASMOperand* divisor, ASMType asmType);

/**
 * @brief Create a Div Instruction object for unsigned division
 * 
 * @param divisor The operand representing the divisor for the DIV instruction
 * @param asmType The type of the division operation (e.g. LONGWORD, QUADWORD)
 * @return ASMInstruction* 
 */
ASMInstruction* createDivInstruction(ASMOperand* divisor, ASMType asmType);


/**
 * @brief Create a MOVZX Instruction object
 * 
 * @param src The source operand for the MOVZX instruction
 * @param dest The destination operand for the MOVZX instruction
 * @return ASMInstruction* pointer to the created MOVZX instruction
 */
ASMInstruction* createASMMovZeroExtendInstruction(ASMOperand* src, ASMOperand* dest);

/**
 * @brief Create a Cmp Instruction object
 * 
 * @param op1 The first operand for the comparison
 * @param op2 The second operand for the comparison
 * @return ASMInstruction* pointer to the created CMP instruction
 */
ASMInstruction* createASMCmpInstruction(ASMType asmType, ASMOperand* op1, ASMOperand* op2);


/**
 * @brief Create a CDQ Instruction object
 * 
 * @param asmType The type of the CDQ instruction (e.g. LONGWORD, QUADWORD)
 * @return ASMInstruction* pointer to the created CDQ instruction
 */
ASMInstruction* createASMCDQInstruction(ASMType asmType);


/**
 * @brief Create a MOVSX Instruction object
 * 
 * @param src The source operand for the MOVSX instruction
 * @param dest The destination operand for the MOVSX instruction
 * @return ASMInstruction* pointer to the created MOVSX instruction
 */
ASMInstruction* createASMMovsxInstruction(ASMOperand* src, ASMOperand* dest);


/**
 * @brief Create a Jump Instruction object
 * 
 * @param label The label to jump to
 * @return ASMInstruction* pointer to the created JUMP instruction
 */
ASMInstruction* createASMJumpInstruction(char* label);

/**
 * @brief Create a Conditional Jump Instruction object
 * 
 * @param cond The condition code for the jump
 * @param label The label to jump to if condition is true
 * @return ASMInstruction* pointer to the created JUMPCC instruction
 */
ASMInstruction* createASMJumpCCInstruction(ASMCondCode cond, char* label);

/**
 * @brief Create a SetCC Instruction object
 * 
 * @param cond The condition code to set
 * @param op The operand where the result will be stored
 * @return ASMInstruction* pointer to the created SETCC instruction
 */
ASMInstruction* createASMSetCCInstruction(ASMCondCode cond, ASMOperand* op);

/**
 * @brief Create a Label Instruction object
 * 
 * @param label The label string for this instruction
 * @return ASMInstruction* pointer to the created LABEL instruction
 */
ASMInstruction* createASMLabelInstruction(char* label);

/**
 * @brief Create a Push Instruction object
 * 
 * @param src The source operand to push onto the stack
 * @return ASMInstruction* pointer to the created PUSH instruction
 */
ASMInstruction* createASMPushInstruction(ASMOperand* src);


/**
 * @brief Create a Pop Instruction object
 * 
 * @param dest The destination operand to pop from the stack
 * @return ASMInstruction* pointer to the created POP instruction
 */
ASMInstruction* createASMPopInstruction(ASMOperand* dest);

/**
 * @brief Create an Allocate Stack Instruction object
 * 
 * @param size The size of the stack to allocate (in bytes)
 * @return ASMInstruction* pointer to the created ALLOCATE STACK instruction
 */
ASMInstruction* createASMAllocateStackInstruction(int size);


/**
 * @brief Create a Deallocate Stack Instruction object
 * 
 * @param size The size of the stack to deallocate (in bytes)
 * @return ASMInstruction* pointer to the created DEALLOCATE STACK instruction
 */
ASMInstruction* createASMDeallocateStackInstruction(int size);

/**
 * @brief Create a Call Instruction object
 * 
 * @param functionName The name of the function to call
 * @return ASMInstruction* pointer to the created CALL instruction
 */
ASMInstruction* createASMCallInstruction(char* functionName);

/**
 * @brief Create a Return Instruction object
 * 
 * @return ASMInstruction* pointer to the created RETURN instruction
 */
ASMInstruction* createASMReturnInstruction();

/**
 * @brief Create a Top Level object
 * 
 * @param type The type of the top-level declaration (function or static variable)
 * @param value A pointer to the value of the top-level declaration (either ASMFunction* or ASMStaticVar*)
 * @return ASMTopLevel* pointer to the created ASMTopLevel
 */
ASMTopLevel* createTopLevel(ASMTopLevelType type, void* value);

/**
 * @brief Create an ASMTopLevel object from a given ASMFunction, encapsulating it as a top-level declaration.
 * 
 * @param func The ASMFunction to be wrapped in an ASMTopLevel
 * @return ASMTopLevel* pointer to the created ASMTopLevel representing the function
 */
ASMTopLevel* createASMTopLevelFromASMFunction(ASMFunction* func);