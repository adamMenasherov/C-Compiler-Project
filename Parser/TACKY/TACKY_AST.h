#pragma once
#include "../AST/C-AST-Nodes/C-ASTNodes.h"
#include "../../DataStructures/DynamicArray/Wrappers/InstructionArrayWrapper.h"
#include "../../DataStructures/DynamicArray/Wrappers/IdentifierWrapper.h"
#include "../../DataStructures/DynamicArray/Wrappers/TACKYTopLevelArrayWrapper.h"
#include "../../DataStructures/DynamicArray/Wrappers/TACKYValueArrayWrapper.h"
#include "../../DataStructures/HashTable/Wrappers/SymbolTableWrapper.h"

typedef enum {
    TACKY_UNARY,
    TACKY_BINARY,
    TACKY_COPY,
    TACKY_JUMP,
    TACKY_JUMP_IF_ZERO,
    TACKY_JUMP_IF_NOT_ZERO,
    TACKY_RETURN,
    TACKY_FUNCTION_CALL,
    TACKY_LABEL
} TACKYInstructionType;

typedef enum {
    TACKY_INT,
    TACKY_LONG
} TACKYStaticVarType;

typedef enum {
    TACKY_STATIC_VAR,
    TACKY_FUNC
} TACKYTopLevelType;

typedef enum {
    TACKY_VAR,
    TACKY_CONSTANT
} TACKYValueType;

typedef struct {
    int value;
} TACKYConstant;

typedef struct TACKYValue {
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
        struct {
            char* functionName;
            TACKYValueArray* args;
            TACKYValue* resultVar;
        } funCall;
    } instValue;
} TACKYInstruction;

typedef InstructionArray TACKYInstructionList;


typedef struct {
    TACKYValue* val;
} TACKYReturn;

typedef struct TACKYFunction {
    char* function_name; 
    IdentifierArray* parameters;
    TACKYInstructionList* instruction_list;
    int global;
} TACKYFunction;

typedef struct {
    char* identifier;
    int global;
    TACKYStaticVarType type;
    int init;
} TACKYStaticVar;

typedef struct TACKYTopLevel {
    TACKYTopLevelType type;
    union {
        TACKYStaticVar* staticVar;
        TACKYFunction* function;
    } topLevel;
} TACKYTopLevel;

typedef struct TACKYProgram {
    TACKYTopLevelArray* topLevels;
} TACKYProgram;


/**
 * Converts a C AST function into a TACKY function representation.
 * @param func The C AST function to convert
 * @return A pointer to the generated TACKYFunction
 */
TACKYFunction* parseTACKYFunction(CDeclaration* func, SymbolTable* symTable);

/**
 * Converts a C AST program into a TACKY program representation.
 * @param program The C AST program to convert
 * @return A pointer to the generated TACKYProgram
 */
TACKYProgram* parseTACKYProgram(CProgram* program, SymbolTable* symTable);

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
 * @brief Parses a C AST if statement and emits corresponding TACKY instructions, adding them to the instruction list.
 * 
 * @param if_stmt The C AST if statement to parse
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseIfStatementInstructions(CIf* if_stmt, TACKYInstructionList* instructionList);


/**
 * @brief Parses a C AST block and emits corresponding TACKY instructions for each block item, adding them to the instruction list.
 * 
 * @param block The C AST block to parse, containing a list of block items (declarations and statements)
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseBlock(CBlock* block, TACKYInstructionList* instructionList);

/**
 * @brief Parses a C AST while loop statement and emits corresponding TACKY instructions, adding them to the instruction list.
 * 
 * @param loop The C AST while loop statement to parse
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseWhileLoopInstructions(CLoop* loop, TACKYInstructionList* instructionList);

/**
 *  @brief Parses a C AST do-while loop statement and emits corresponding TACKY instructions, adding them to the instruction list.
 * 
 * @param loop The C AST do-while loop statement to parse
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseDoWhileLoopInstructions(CLoop* loop, TACKYInstructionList* instructionList);

/**
 * @brief Parses a C AST for loop statement and emits corresponding TACKY instructions, adding them to the instruction list.
 * 
 * @param forLoop The C AST for loop statement to parse
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseForLoopInstructions(CForLoop* forLoop, TACKYInstructionList* instructionList);

/**
 * @brief Parses the initialization part of a C AST for loop statement and emits corresponding TACKY instructions, adding them to the instruction list.
 * 
 * @param init The C AST for loop initialization to parse
 * @param instructionList The list to which generated TACKY instructions will be added
 */
void parseForLoopInitInstructions(CForInit* init, TACKYInstructionList* instructionList);

/**
 * @brief Converts the symbol table entries for global variables into TACKY top-level variable declarations, adding them to the provided top-level array.
 * 
 * @param symTable The symbol table containing identifier information, including global variables
 * @param topLevels The TACKY top-level array to which generated TACKYVariable top-level nodes will be added for each global variable found in the symbol table
 */
void convertSymbolsToTACKY(IdentifierTypeInfo* symTable, void* userData);