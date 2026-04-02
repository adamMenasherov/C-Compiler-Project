#pragma once
#include "../AST/C-AST-Nodes/C-ASTNodes.h"

typedef enum {
    TACKY_UNARY,
    TACKY_BINARY
} TACKYInstructionType;

typedef enum {
    TACKY_VAR,
    TACKY_CONSTANT
} TACKYValueType;

typedef struct {
    int value;
} TACKYConstant;

typedef struct {
    TACKYValueType type;
    TACKYConstant* constant;
    char* identifier; 
} TACKYValue;

typedef struct TACKYInstruction {
    TACKYInstructionType type;
    union {
        struct {
            unaryType type;
            TACKYValue* src;
            TACKYValue* dest;
        } unaryOp;
        struct {
            binType binaryOpType;
            TACKYValue* src1;
            TACKYValue* src2;
            TACKYValue* dest;
        } binaryOp;
    } instValue;
} TACKYInstruction;

typedef struct {
    TACKYInstruction** instructions;
    int arrSize;
    int currSize;
    int cursor;
} TACKYInstructionList;


typedef struct {
    TACKYValue* val;
} TACKYReturn;

typedef struct {
    char* function_name; 
    TACKYReturn* inst; 
    TACKYInstructionList* instruction_list;
} TACKYFunction;

typedef struct {
    TACKYFunction* function_def;
} TACKYProgram;


/**
 * Converts a C AST function into a TACKY function representation.
 * @param func The C AST function to convert
 * @return A pointer to the generated TACKYFunction
 */
TACKYFunction* parseTACKYFunction(CFunction* func);

/**
 * Converts a C AST program into a TACKY program representation.
 * @param program The C AST program to convert
 * @return A pointer to the generated TACKYProgram
 */
TACKYProgram* parseTACKYProgram(CProgram* program);

/**
 * Converts a C AST return statement into a TACKY return representation.
 * @param returnNode The C AST return statement to convert
 * @return A pointer to the generated TACKYReturn
 */
TACKYReturn* parseTACKYReturn(CReturn* returnNode, TACKYInstructionList* instructionList);

/**
 * Emits TACKY instructions from a C expression and adds them to the instruction list.
 * @param exp The C AST expression to emit instructions for
 * @param instruction_list The list to add generated instructions to
 * @return A TACKYValue representing the result of the expression
 */
TACKYValue* emit_TACKY(CFactor* exp, TACKYInstructionList* instruction_list);

/**
 * Creates a unary operation instruction.
 * @param type The type of unary operation
 * @param src The source operand
 * @param dest The destination operand where result is stored
 * @return A pointer to the created TACKYInstruction
 */
TACKYInstruction* createUnaryInstruction(unaryType type, TACKYValue* src, TACKYValue* dest);


/**
 * @brief Creates a binary operation instruction.
 * @param type The type of binary operation
 * @param src1 The first source operand
 * @param src2 The second source operand
 * @param dest The destination operand where result is stored
 * @return A pointer to the created TACKYInstruction
 */
TACKYInstruction* createBinaryInstruction(binType type, TACKYValue* src1, TACKYValue* src2, TACKYValue* dest);

/**
 * @brief Create a Tacky Value From C Constant Node object
 * 
 * @param constantNode the C constantNode
 * @return TACKYValue* pointer to tackyValue
 */
TACKYValue* createTackyValueFromConstantNode(CConstant* const_node);


/**
 * @brief Create a Var Value object
 * 
 * @return TACKYValue* the Var Value object
 */
TACKYValue* createVarValue(char* identifier);


/**
 * Initializes and creates a new TACKY instruction list.
 * @return A pointer to the newly created TACKYInstructionList
 */
TACKYInstructionList* createTACKYInstructionList();

/**
 * @brief Create a Tacky Constant Node object
 * 
 * @param val The integer value
 * @return a pointer to the new constant node
 */
TACKYConstant* CreateTackyConstantNode(int val);

/**
 * Adds an instruction to the TACKY instruction list, expanding if necessary.
 * @param list The instruction list to add to
 * @param instruction The instruction to add
 */
void addInstructionToList(TACKYInstructionList* list, TACKYInstruction* instruction);

/**
 * @brief Generates temporary var name
 * 
 * @return char* temp var name
 */
char* generateTempName();

/**
 * @brief Creates a deep copy of a TACKYValue
 * 
 * @param original The TACKYValue to copy
 * @return TACKYValue* A new copy, or NULL if original is NULL
 */
TACKYValue* copyTackyValue(TACKYValue* original);
