#pragma once

typedef enum {
    FACTOR_CONSTANT,
    FACTOR_UNARY,
    FACTOR_BINARY,
    FACTOR_VAR,
    FACTOR_ASSIGNMENT
} factorType;

typedef enum {
    UNARY_COMPLEMENT,
    UNARY_NEGATE,
    UNARY_NOT,
    UNARY_NOT_UNARY_OP
} unaryType;

typedef enum {
    BIN_SUBTRACT,
    BIN_ADD,
    BIN_MULTIPLY,
    BIN_DIVIDE,
    BIN_REMAINDER,
    BIN_AND,
    BIN_OR,
    BIN_EQUALS,
    BIN_NOT_EQUALS,
    BIN_LESS_THAN,
    BIN_LESS_EQUAL,
    BIN_GREATER_THAN,
    BIN_GREATER_EQUAL,
    BIN_NOT_BINARY_OP
} binType;

typedef enum {
    STMT_RETURN,
    STMT_EXPRESSION,
    STMT_NULL
} statementType;

typedef enum {
    DECL_WITH_EXP,
    DECL_WITHOUT_EXP
} declerationType;

typedef enum {
    BLOCK_ITEM_DECL,
    BLOCK_ITEM_STMT
} blockItemType;

typedef struct {
    int val; 
} CConstant;

typedef struct CBinary CBinary;
typedef struct CUnary CUnary;
typedef struct CFactor CFactor;

typedef struct {
    char * identifier;
} CVar;

typedef struct {
    CFactor* exp1;
    CFactor* exp2;
} CAssignment;


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
        CVar * var;
        CAssignment * assignment;
    } exp;
} CFactor; 

typedef struct CUnary {
    CFactor * exp;
    unaryType type;
} CUnary;

/* Type definitions */
typedef struct {
    CFactor* exp;
} CReturn;

typedef struct {
    statementType type;
    union {
        CReturn* ret;
        CFactor* exp;
    } stmt;
} CStatement;

typedef struct {
    declerationType declType;
    char * identifier;
    CFactor* exp;
} CDeclaration;

typedef struct {
    blockItemType type;
    union {
        CDeclaration* decl;
        CStatement* stmt;
    } item;
} CBlockItem;


