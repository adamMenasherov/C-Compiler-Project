#pragma once
#include "../Common/SharedNodeEnums.h"
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
    TACKY_LABEL,
    TACKY_SIGN_EXTEND,
    TACKY_ZERO_EXTEND,
    TACKY_DOUBLE_TO_INT,
    TACKY_DOUBLE_TO_UINT,
    TACKY_INT_TO_DOUBLE,
    TACKY_UINT_TO_DOUBLE,
    TACKY_TRUNCATE
} TACKYInstructionType;

typedef enum {
    TACKY_INT,
    TACKY_LONG,
    TACKY_DOUBLE
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
    constantType type; 
    union {
        uint64_t intValue;
        double doubleValue;
    } val;
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
        struct {
            TACKYValue* src;
            TACKYValue* dest;
        } signExtend;
        struct {
            TACKYValue* src;
            TACKYValue* dest;
        } zeroExtend;
        struct {
            TACKYValue* src;
            TACKYValue* dest;
        } truncate;
        struct {
            TACKYValue* src;
            TACKYValue* dest;
        } doubleIntCast;
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
    staticInitialVal initVal;
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

TACKYValue* makeTACKYVariable(specifierType type, SymbolTable* symTable);