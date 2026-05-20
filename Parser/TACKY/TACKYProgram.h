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
    TACKY_CONSTANT,
    TACKY_STATIC
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