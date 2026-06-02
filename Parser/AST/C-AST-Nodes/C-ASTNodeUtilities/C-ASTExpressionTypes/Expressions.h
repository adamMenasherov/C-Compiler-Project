#pragma once

typedef enum {
    FACTOR_CONSTANT,
    FACTOR_UNARY,
    FACTOR_BINARY,
    FACTOR_VAR,
    FACTOR_ASSIGNMENT,
    FACTOR_CONDITIONAL,
    FACTOR_FUNCTION_CALL,
    FACTOR_CAST,
    FACTOR_DEREFERENCE,
    FACTOR_ADDRESS_OF
} factorType;

typedef struct {
    constantType type;
    union {
        int64_t intValue;
        double doubleValue;
    } value;
} CConstant;

typedef struct {
    char* identifier;
} CVar;

typedef struct {
    CFactor* exp1;
    CFactor* exp2;
} CAssignment;

typedef struct {
    CFactor* condition;
    CFactor* then;
    CFactor* else_stmt;
} CConditional;

typedef struct CBinary {
    CFactor* left;
    CFactor* right;
    binType type;
} CBinary;

typedef struct {
    char* identifier;
    ExpressionFactorArray* arguments;
} CFunctionCall;

typedef struct {
    CType* targetType;
    CFactor* exp;
} CCast;


typedef struct CFactor {
    factorType type;
    CType* valueType;
    union {
        CConstant* cnst;
        CUnary* unary;
        CBinary* binary;
        CVar* var;
        CAssignment* assignment;
        CConditional* conditional;
        CFunctionCall* funcCall;
        CCast* cast;
        CFactor* pointerOp;
    } exp;
} CFactor;

typedef struct CUnary {
    CFactor* exp;
    unaryType type;
} CUnary;
