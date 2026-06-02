#pragma once
typedef struct CDeclarator CDeclarator;

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
    DECLARATOR_IDENT,
    DECLARATOR_POINTER,
    DECLARATOR_FUNC
} declaratorType;

typedef struct {
    CType* type;
    CDeclarator* decl;
} CParamInfo;

typedef struct CDeclarator {
    declaratorType type;
    union {
        char* identifier;
        struct {
            struct CDeclarator* pointed;
        } pointerDecl;
        struct {
            struct CDeclarator* funcDecl;
            CParamInfo params[MAX_PARAMS];
            int paramCnt;
        } func;
    } decl;
} CDeclarator;

typedef struct CCompound {
    CBlock* block;
} CCompound;

typedef struct CBlock {
    CBlockItemList* items;
} CBlock;

typedef struct CDeclaration {
    declType type;
    union {
        struct {
            varDeclType declType;
            CType* varType;
            StorageClass storageClass;
            char* identifier;
            CFactor* exp;
        } variableDecl;
        struct {
            funcDeclType declType;
            CType* funcType;
            StorageClass storageClass;
            char* identifier;
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
