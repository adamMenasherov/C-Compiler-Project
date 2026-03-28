#pragma once
#include "../C-AST-Nodes/C-ASTNodes.h"

typedef enum {
    TACKY_RETURN,
    TACKY_UNARY
} TACKYInstructionType;

typedef enum {
    TACKY_VAR,
    TACKY_CONSTANT
} TACKYValueType;

typedef struct {
    TACKYFunction* function_def;
} TACKYProgram;

typedef struct {
    int value;
} TACKYConstant;

typedef struct {
    TACKYValueType type;
    TACKYConstant* constant;
    char* identifier; 
} TACKYValue;

typedef struct {
    char* function_name; 
    TACKYInstruction* inst; 
} TACKYFunction;


typedef struct TACKYInstruction {
    TACKYInstructionType type;
    union {
        union {
          TACKYValue * val;
        } returnVal;
        union {
            unaryType type;
            TACKYValue* src;
            TACKYValue* dest;
        } unaryOp;
    } instValue;
    TACKYInstruction* next;
} TACKYInstruction;