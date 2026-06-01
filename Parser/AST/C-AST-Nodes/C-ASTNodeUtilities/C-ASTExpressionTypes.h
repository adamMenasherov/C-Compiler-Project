#pragma once
#include <stdint.h>
#define MAX_PARAMS 128
#define MAX_CASES 1024

typedef struct CBlockItem CBlockItem;
typedef struct CFactor CFactor;

#include "../../../../DataStructures/DynamicArray/Wrappers/BlockItemArrayWrapper.h"
#include "../../../../DataStructures/DynamicArray/Wrappers/IdentifierWrapper.h"
#include "../../../../DataStructures/DynamicArray/Wrappers/ExpressionFactorWrapper.h"
#include "../../../Common/SharedNodeEnums.h"
typedef BlockItemArray CBlockItemList;

typedef enum {
    FACTOR_CONSTANT,
    FACTOR_UNARY,
    FACTOR_BINARY,
    FACTOR_VAR,
    FACTOR_ASSIGNMENT,
    FACTOR_CONDITIONAL,
    FACTOR_FUNCTION_CALL,
    FACTOR_CAST,
    FACTOR_POINTER_OP
} factorType;

typedef enum {
    SPEC_LONG,
    SPEC_INT,
    SPEC_UNSIGNED_INT,
    SPEC_UNSIGNED_LONG,
    SPEC_DOUBLE,
    SPEC_STATIC,
    SPEC_EXTERN,
    SPEC_POINTER,
    SPEC_NULL
} specifierType;

typedef struct {
    unsigned int isLong : 1;
    unsigned int isInt : 1;
    unsigned int isDouble : 1;
    unsigned int isSigned : 1;
    unsigned int isUnsigned : 1;
    unsigned int isStatic : 1;
    unsigned int isExtern : 1;
} TypeFlags;


typedef enum {
    IF_WITHOUT_ELSE,
    IF_WITH_ELSE
} ifType;

typedef enum {
    STMT_RETURN,
    STMT_EXPRESSION,
    STMT_IF,
    STMT_COMPOUND,
    STMT_BREAK,
    STMT_SWITCH,
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

typedef enum {
    POINTER_DEREFERENCE,
    POINTER_ADDRESS_OF
} pointerOp;

typedef struct CBinary CBinary;
typedef struct CUnary CUnary;
typedef struct CFactor CFactor;
typedef struct CStatement CStatement;
typedef struct CBlock CBlock;
typedef struct CDeclaration CDeclaration;
typedef struct CCompound CCompound;

typedef struct {
    constantType type;
    union {
        int64_t intValue;
        double doubleValue;
    } value;
} CConstant;

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

typedef struct CFuncType {
    specifierType type;
    struct {
        struct CFuncType* params[MAX_PARAMS];
        int paramCnt;
    } func;
} CFuncType;

typedef struct {
    specifierType targetType;
    CFactor* exp;
} CCast;

typedef struct {
    pointerOp type;
    CFactor* exp;
} CPointerOp;

typedef struct CFactor {
    factorType type;
    specifierType valueType; // For type checking in semantic analysis
    union {
        CConstant * cnst;
        CUnary * unary;
        CBinary * binary;
        CVar * var;
        CAssignment * assignment;
        CConditional * conditional;
        CFunctionCall* funcCall;
        CCast* cast;
        CPointerOp* pointerOp;
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
    CConstant* matchVal;
    CStatement* body;
    int hasBreak;
} CCase;


typedef struct {
    CFactor* switchExp;
    CCase* cases[MAX_CASES];
    int caseCount;
    CStatement* defaultCase;
} CSwitch;

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
        CSwitch* switch_stmt;
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
            CFuncType* funcType;
            specifierType storageClass;
            char * identifier;
            IdentifierArray* parameters;
            CBlock* body;
        } functionDecl;
        struct {
            CDeclaration* referenced;
        } pointerDecl;
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