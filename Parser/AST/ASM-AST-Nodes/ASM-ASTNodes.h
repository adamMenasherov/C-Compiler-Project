#pragma once
#include "../C-AST-Nodes/C-ASTNodes.h"
#include "../../TACKY/TACKY_AST.h"
#include "../../../DataStructures/HashTable.h"

typedef enum {
    ASM_UNARY,
    ASM_ALLOCATESTACK,
    ASM_MOV,
    ASM_BINARY,
    ASM_CDQ,
    ASM_IDIV,
    ASM_RET
} ASMInstructionType;

typedef enum {
    ASM_UNARY_NEG,
    ASM_UNARY_NOT,
} ASMUnaryOperator;

typedef enum {
    ASM_BINARY_ADD,
    ASM_BINARY_SUBTRACT,
    ASM_BINARY_MULTIPLY,
} ASMBinaryOperator;

typedef enum {
    ASM_OP_REGISTER,
    ASM_OP_IMMEDIATE,
    ASM_OP_PSEUDO,
    ASM_OP_STACK
} OperandType;

typedef enum {
    AX,
    DX,
    R10,
    R11
} Register;

typedef struct {
    OperandType type;
    union {
        Register reg;
        int immediate;
        char* identifier;
    } OperandValue;
} ASMOperand;


typedef struct ASMInstruction {
    ASMInstructionType type;
    union {
        struct {
            ASMOperand* operand1;
            ASMOperand* operand2;
        } mov;
        struct {
            ASMUnaryOperator type;
            ASMOperand* op;
        } unary;
        struct {
            int size;
        } allocatestack;
        struct {
            ASMBinaryOperator type;
            ASMOperand* op1;
            ASMOperand* op2;
        } binary;
        struct {
            ASMOperand* divisor;
        } idiv;
    } instValue;
    struct ASMInstruction* next;
} ASMInstruction;

typedef struct {
    ASMInstruction* head;
    ASMInstruction* tail;
} ASMInstructionList;

typedef struct ASMFunction {
    char* function_name;
    ASMInstructionList* inst;
    HashTable* pseudoTable; // Maps pseudo-register names to stack offsets
} ASMFunction;


typedef struct {
    ASMFunction* function_def;
} ASMProgram;

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
 * Emits the ASM instructions for a TACKY return statement:
 * moves the return value into EAX and appends a RET instruction.
 * @param tacky_ret The TACKY return node to convert
 * @param instruction_list The ASM instruction list to append to
 */
void parseASMReturn(TACKYReturn* tacky_ret, ASMInstructionList* instruction_list);

/**
 * Converts a list of TACKY instructions into ASM instructions,
 * appending each result to the given ASM instruction list.
 * @param tackyInstList The source TACKY instruction list
 * @param asmInstructionList The destination ASM instruction list
 */
void parseASMInstruction(TACKYInstructionList* tackyInstList, ASMInstructionList* asmInstructionList);

/**
 * @brief Converts a TACKYValue into an ASMOperand, handling both constants and variables.
 * 
 * @param val 
 * @return ASMOperand* pointer to the created ASMOperand
 */
ASMOperand* tackyValueToASMOperand(TACKYValue* val);


/**
 * @brief Frees the memory allocated for an ASMProgram, including all nested structures.
 * 
 * @param program The ASMProgram to free
 */
ASMOperand* createRegisterOperand(Register reg);


/**
    * @brief Creates an ASMInstruction representing a unary operation (e.g. NEG, NOT).
 * 
 * @param type The type of unary operation
 * @param src The source operand for the unary operation
 * @param dest The destination operand where the result will be stored
 * @return A pointer to the created ASMInstruction
 */
ASMInstruction* createASMUnaryInstruction(unaryType type, TACKYValue* dest);


/**
 * @brief Creates an ASMInstruction representing a binary operation (e.g. ADD, SUB, MUL).
 * 
 * @param type The type of binary operation
 * @param op1 The first operand for the binary operation
 * @param op2 The second operand for the binary operation
 * @return A pointer to the created ASMInstruction
 */
ASMInstruction* createASMBinaryInstruction(binType type, ASMOperand* op1, ASMOperand* op2);

/**
 * @brief Create a Mov Instruction object
 * 
 * @param src The source operand for the MOV instruction
 * @param dest The destination operand for the MOV instruction
 * @return ASMInstruction* pointer to the created MOV instruction 
 */
ASMInstruction* createMovInstruction(ASMOperand* src, ASMOperand* dest);


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
ASMInstruction* CreateIdivInstruction(ASMOperand* divisor);

/**
 * @brief Create a Stack Operand object
 * 
 * @param offset The offset of the stack location
 * @return ASMOperand* pointer to the created stack operand
 */
ASMOperand* createStackOperand(int offset);


/**
 * @brief Handles the special case for divide and modulo operations.
 * 
 * @param instruction The TACKY instruction for the divide or modulo operation
 * @param asmInstructionList The ASM instruction list to append to
 */
void handleDivideModuloCase(TACKYInstruction* instruction, ASMInstructionList* asmInstructionList);