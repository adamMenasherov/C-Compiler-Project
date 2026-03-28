#pragma once

typedef enum {
    EXP_CONSTANT,
    EXP_UNARY
} expType;

typedef enum {
    COMPLEMENT,
    NEGATE,
    NOT_UNARY_OP
} unaryType;

typedef struct {
    int val; 
} CConstant;

/* Forward declaration */
typedef struct CExpression CExpression;

typedef struct {
    CExpression * exp;
    unaryType type;
} CUnary;

typedef struct CExpression {
    expType type;
    union {
        CConstant * cnst;
        CUnary * unary;
    } exp;
} CExpression; 
