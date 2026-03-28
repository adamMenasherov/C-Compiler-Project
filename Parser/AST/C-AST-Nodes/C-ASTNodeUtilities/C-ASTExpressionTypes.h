#pragma once

typedef enum {
    EXP_CONSTANT,
    EXP_UNARY
} expType;

typedef enum {
    UNARY_COMPLEMENT,
    UNARY_NEGATE,
    UNARY_NOT_UNARY_OP
} unaryType;

typedef struct {
    int val; 
} CConstant;

typedef struct CUnary CUnary;

typedef struct CExpression {
    expType type;
    union {
        CConstant * cnst;
        CUnary * unary;
    } exp;
} CExpression; 


typedef struct CUnary {
    CExpression * exp;
    unaryType type;
} CUnary;


