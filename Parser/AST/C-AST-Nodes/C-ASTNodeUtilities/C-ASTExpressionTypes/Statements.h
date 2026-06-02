#pragma once
typedef struct CStatement CStatement;

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
    FOR_INIT_DECL,
    FOR_INIT_EXP,
    FOR_INIT_WITHOUT
} forInitType;

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
    char* identifier;
} CLoopStmt;

typedef struct {
    CConstant*  matchVal;
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
};
