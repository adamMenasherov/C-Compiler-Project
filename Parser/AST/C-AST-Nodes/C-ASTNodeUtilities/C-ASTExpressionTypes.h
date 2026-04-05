#pragma once

typedef struct CBlockItem CBlockItem;
#include "../../../../DataStructures/DynamicArray/Wrappers/BlockItemArrayWrapper.h"
typedef BlockItemArray CBlockItemList;

typedef enum {
    FACTOR_CONSTANT,
    FACTOR_UNARY,
    FACTOR_BINARY,
    FACTOR_VAR,
    FACTOR_ASSIGNMENT,
    FACTOR_CONDITIONAL
} factorType;

typedef enum {
    UNARY_COMPLEMENT,
    UNARY_NEGATE,
    UNARY_NOT,
    UNARY_INCREMENT,
    UNARY_DECREMENT,
    UNARY_INCREMENT_PREFIX,
    UNARY_INCREMENT_POSTFIX,
    UNARY_DECREMENT_PREFIX,
    UNARY_DECREMENT_POSTFIX,
    UNARY_NOT_UNARY_OP
} unaryType;

typedef enum {
    IF_WITHOUT_ELSE,
    IF_WITH_ELSE
} ifType;


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
    BIN_NOT_BINARY_OP,
    BIN_BITWISE_AND,
    BIN_BITWISE_OR,
    BIN_BITWISE_XOR,
    BIN_LEFT_SHIFT,
    BIN_RIGHT_SHIFT
} binType;

typedef enum {
    STMT_RETURN,
    STMT_EXPRESSION,
    STMT_IF,
    STMT_COMPOUND,
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
typedef struct CStatement CStatement;
typedef struct CBlock CBlock;
typedef struct CCompound CCompound;

typedef struct {
    char * identifier;
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

typedef struct CCompound {
    CBlock* block;
} CCompound;

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
        CConditional * conditional;
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
    ifType type;
    CFactor* condition;
    CStatement* then;
    CStatement* else_stmt; 
} CIf;

typedef struct CStatement {
    statementType type;
    union {
        CReturn* ret;
        CFactor* exp;
        CIf* if_stmt;
        CCompound* compound_stmt;
    } stmt;
} CStatement;

typedef struct {
    declerationType declType;
    char * identifier;
    CFactor* exp;
} CDeclaration;

typedef struct CBlockItem {
    blockItemType type;
    union {
        CDeclaration* decl;
        CStatement* stmt;
    } item;
} CBlockItem;

typedef struct CBlock {
    CBlockItemList* items;
} CBlock;


