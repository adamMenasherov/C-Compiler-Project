#pragma once

/* ── Enums ─────────────────────────────────────────────────────────── */

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
    POINTER_DEREFERENCE,
    POINTER_ADDRESS_OF
} pointerOp;

/* ── Leaf nodes ─────────────────────────────────────────────────────── */

typedef struct {
    constantType type;
    union {
        int64_t intValue;
        double  doubleValue;
    } value;
} CConstant;

typedef struct {
    char* identifier;
} CVar;

/* ── Compound expression nodes ──────────────────────────────────────── */

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
    binType  type;
} CBinary;

typedef struct {
    char*                identifier;
    ExpressionFactorArray* arguments;
} CFunctionCall;

typedef struct CFuncType {
    specifierType type;
    struct {
        struct CFuncType* params[MAX_PARAMS];
        int               paramCnt;
    } func;
} CFuncType;

typedef struct {
    specifierType targetType;
    CFactor*      exp;
} CCast;

typedef struct {
    pointerOp type;
    CFactor*  exp;
} CPointerOp;

/* ── CFactor (the polymorphic expression node) ─────────────────────── */

typedef struct CFactor {
    factorType    type;
    specifierType valueType;
    union {
        CConstant*     cnst;
        CUnary*        unary;
        CBinary*       binary;
        CVar*          var;
        CAssignment*   assignment;
        CConditional*  conditional;
        CFunctionCall* funcCall;
        CCast*         cast;
        CPointerOp*    pointerOp;
    } exp;
} CFactor;

typedef struct CUnary {
    CFactor*   exp;
    unaryType  type;
} CUnary;
