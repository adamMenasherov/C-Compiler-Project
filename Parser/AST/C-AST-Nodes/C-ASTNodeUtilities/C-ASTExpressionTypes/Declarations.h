#pragma once

/* ── Enums ─────────────────────────────────────────────────────────── */

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

/* ── Block / compound ───────────────────────────────────────────────── */

typedef struct CCompound {
    CBlock* block;
} CCompound;

typedef struct CBlock {
    CBlockItemList* items;
} CBlock;

/* ── Declarations ────────────────────────────────────────────────────── */

typedef struct CDeclaration {
    declType type;
    union {
        struct {
            varDeclType   declType;
            specifierType varType;
            specifierType storageClass;
            char*         identifier;
            CFactor*      exp;
        } variableDecl;
        struct {
            funcDeclType  declType;
            CFuncType*    funcType;
            specifierType storageClass;
            char*         identifier;
            IdentifierArray* parameters;
            CBlock*       body;
        } functionDecl;
        struct {
            CDeclaration* referenced;
        } pointerDecl;
    } decl;
} CDeclaration;

/* ── Block items (declaration or statement) ─────────────────────────── */

typedef struct CBlockItem {
    blockItemType type;
    union {
        CDeclaration* decl;
        CStatement*   stmt;
    } item;
} CBlockItem;
