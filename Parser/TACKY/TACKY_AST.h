#pragma once
#include "../AST/C-AST-Nodes/C-ASTNodes.h"
#include "../../DataStructures/DynamicArray/Wrappers/InstructionArrayWrapper.h"

typedef enum {
    TACKY_UNARY,
    TACKY_BINARY,
    TACKY_COPY,
    TACKY_JUMP,
    TACKY_JUMP_IF_ZERO,
    TACKY_JUMP_IF_NOT_ZERO,
    TACKY_RETURN,
    TACKY_LABEL
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
        struct {
            char* label;
        } jump;
        struct {
            char* label;
            TACKYValue* condition;
        } condJump;
        struct {
            char* label;
        } label;
        struct {
            TACKYValue* src;
            TACKYValue* dest;
        } copy;
        struct {
            TACKYValue* retVal;
        } returnVal;
    } instValue;
} TACKYInstruction;

typedef InstructionArray TACKYInstructionList;


typedef struct {
    TACKYValue* val;
} TACKYReturn;

typedef struct {
    char* function_name; 
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
void parseTACKYReturn(CReturn* returnNode, TACKYInstructionList* instructionList);

/**
 * @brief Parses a C AST block item and emits corresponding TACKY instructions, adding them to the instruction list.
 * 
 * @param blockItem The C AST block item to parse (could be a declaration or statement)
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseBlockItemInstructions(CBlockItem* blockItem, TACKYInstructionList* instructionList);


/**
 * @brief Parses a C AST statement and emits corresponding TACKY instructions, adding them to the instruction list.
 * 
 * @param stmt The C AST statement to parse (could be an expression statement, return statement, etc.)
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseStatementInstructions(CStatement* stmt, TACKYInstructionList* instructionList);

/**
 * Emits TACKY instructions from a C expression and adds them to the instruction list.
 * @param exp The C AST expression to emit instructions for
 * @param instruction_list The list to add generated instructions to
 * @return A TACKYValue representing the result of the expression
 */
TACKYValue* emit_TACKY(CFactor* exp, TACKYInstructionList* instruction_list);

/**
 * @brief Handles short-circuit evaluation for logical operators in TACKY.
 * 
 * @param exp The C AST expression representing the logical operation 
 * @param instruction_list The list to add generated instructions to
 * @return TACKYValue* representing the result of the short-circuit evaluation
 */
TACKYValue* shortCircuitTACKYInstruction(CFactor* exp, TACKYInstructionList* instruction_list);

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
 * @brief Create a Return Instruction object
 * 
 * @param retVal The TACKYValue representing the return value
 * @return TACKYInstruction* the created return instruction
 */
TACKYInstruction* createReturnInstruction(TACKYValue* retVal);

/**
 * @brief Create a Jump Instruction object
 * 
 * @param jumpType The type of jump instruction (e.g., TACKY_JUMP, TACKY_JUMP_IF_ZERO)
 * @param label The label to jump to
 * @return TACKYInstruction* the created jump instruction
 */
TACKYInstruction* createJumpInstruction(TACKYInstructionType jumpType, char* label, TACKYValue* condition);

/**
 * @brief Create a Label Instruction object
 * 
 * @param label The label name for the instruction
 * @return TACKYInstruction* the created label instruction 
 */
TACKYInstruction* createLabelInstruction(char* label);

/**
 * @brief Create a Copy Instruction object
 * 
 * @param src The source TACKYValue to copy from
 * @param dest The destination TACKYValue to copy to
 * @return TACKYInstruction* the created copy instruction
 */
TACKYInstruction* createCopyInstruction(TACKYValue* src, TACKYValue* dest);

/**
 * @brief Create a Tacky Value From Constant object
 * 
 * @param val The integer value to create the TACKY constant from
 * @return TACKYValue* the created TACKYValue representing the constant
 */
TACKYValue* createTackyValueFromConstant(int val);

/**
 * @brief Create a Tacky Value From Var object
 * 
 * @param var The C AST variable to convert
 * @return TACKYValue* The created TACKYValue representing the variable
 */
TACKYValue* createTackyValueFromVar(CVar* var);

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
 * @brief Generates a false label
 * @return char* The generated false label
 */
char* generateFalseLabel();

/**
 * @brief Generates a true label
 * @return char* The generated true label
 */
char* generateTrueLabel();

/**
 * @brief Generates an end label
 * @return char* The generated end label
 */
char* generateEndLabel();

/**
 * @brief Creates a deep copy of a TACKYValue
 * 
 * @param original The TACKYValue to copy
 * @return TACKYValue* A new copy, or NULL if original is NULL
 */
TACKYValue* copyTackyValue(TACKYValue* original);
