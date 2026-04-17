#pragma once

typedef struct CBlockItem CBlockItem;
typedef struct CFactor CFactor;

#include "../../../../DataStructures/DynamicArray/Wrappers/BlockItemArrayWrapper.h"
#include "../../../../DataStructures/DynamicArray/Wrappers/IdentifierWrapper.h"
#include "../../../../DataStructures/DynamicArray/Wrappers/ExpressionFactorWrapper.h"
typedef BlockItemArray CBlockItemList;

typedef enum {
    FACTOR_CONSTANT,
    FACTOR_UNARY,
    FACTOR_BINARY,
    FACTOR_VAR,
    FACTOR_ASSIGNMENT,
    FACTOR_CONDITIONAL,
    FACTOR_FUNCTION_CALL
} factorType;

typedef enum {
    SPEC_INT,
    SPEC_STATIC,
    SPEC_EXTERN,
    SPEC_NULL
} specifierType;

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
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_WHILE,
    STMT_FOR,
    STMT_DO_WHILE,
    STMT_NULL
} statementType;

typedef enum {
    VAR_DECL_WITH_EXP,
    VAR_DECL_WITHOUT_EXP
} varDeclType;

typedef enum {
    DECL_VAR,
    DECL_FUNC
} declType;

typedef enum {
    FUNC_DECL,
    FUNC_DEF
} funcDeclType;

typedef enum {
    BLOCK_ITEM_DECL,
    BLOCK_ITEM_STMT
} blockItemType;

typedef enum {
    FOR_INIT_DECL, 
    FOR_INIT_EXP,
    FOR_INIT_WITHOUT
} forInitType;

typedef struct {
    int val; 
} CConstant;

typedef struct CBinary CBinary;
typedef struct CUnary CUnary;
typedef struct CFactor CFactor;
typedef struct CStatement CStatement;
typedef struct CBlock CBlock;
typedef struct CDeclaration CDeclaration;
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

typedef struct {
    char * identifier;
    ExpressionFactorArray* arguments;
} CFunctionCall;

typedef struct CFactor {
    factorType type;
    union {
        CConstant * cnst;
        CUnary * unary;
        CBinary * binary;
        CVar * var;
        CAssignment * assignment;
        CConditional * conditional;
        CFunctionCall* funcCall;
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

typedef struct {
    forInitType type;
    union {
        CDeclaration* decl;
        CFactor* exp;
    };
} CForInit;


typedef struct {
    CFactor* condition;
    CStatement* body;
    char* identifier;
} CLoop;

typedef struct {
    CForInit* init;
    CFactor* condition;
    CFactor* post;
    CStatement* body;
    char* identifier;
} CForLoop;


typedef struct {
    char* identifier;
} CLoopStmt;

typedef struct CStatement {
    statementType type;
    union {
        CReturn* ret;
        CFactor* exp;
        CIf* if_stmt;
        CCompound* compound_stmt;

        CLoop* while_stmt;
        CLoop* do_while_stmt;
        CForLoop* for_stmt;

        CLoopStmt* break_stmt;
        CLoopStmt* continue_stmt;
    } stmt;
} CStatement;

typedef struct CDeclaration {
    declType type;
    union {
        struct {
            varDeclType declType;
            specifierType varType;
            specifierType storageClass;
            char * identifier;
            CFactor* exp;
        } variableDecl;
        struct {
            funcDeclType declType;
            specifierType funcType;
            specifierType storageClass;
            char * identifier;
            IdentifierArray* parameters;
            CBlock* body;
        } functionDecl;
    } decl;
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


