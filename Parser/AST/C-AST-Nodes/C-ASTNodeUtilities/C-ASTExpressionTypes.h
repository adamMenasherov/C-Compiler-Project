#pragma once

typedef enum {
    FACTOR_CONSTANT,
    FACTOR_UNARY,
    FACTOR_BINARY
} factorType;

typedef enum {
    UNARY_COMPLEMENT,
    UNARY_NEGATE,
    UNARY_NOT_UNARY_OP
} unaryType;

typedef enum {
    BIN_SUBTRACT,
    BIN_ADD,
    BIN_MULTIPLY,
    BIN_DIVIDE,
    BIN_MODULO,
    BIN_NOT_BINARY_OP
} binType;

typedef struct {
    int val; 
} CConstant;

typedef struct CBinary CBinary;
typedef struct CUnary CUnary;
typedef struct CFactor CFactor;


typedef struct CBinary {
    CFactor * left;
    CFactor * right;
    binType type;
} CBinary;

typedef struct CFactor {
    factorType type;
    union {
        CConstant * cnst;
        CUnary * unary;
        CBinary * binary;
    } exp;
} CFactor; 

typedef struct CUnary {
    CFactor * exp;
    unaryType type;
} CUnary;




