#include "TypeChecker.h"
#include "../DeclarationTypeChecker/DeclarationTypeChecker.h"
#include "../StatementTypeChecker/StatementTypeChecker.h"
#include "../../../../DataStructures/DynamicArray/Wrappers/BlockItemArrayWrapper.h"
#include <stdlib.h>

/* ---- Declaration type-check handlers ---- */

static void handleDeclVar(CDeclaration* decl, SymbolTable* symbolTable) {
    typeCheckLocalVariableDeclaration(decl, symbolTable);
}

static void handleDeclFunc(CDeclaration* decl, SymbolTable* symbolTable) {
    typeCheckFunctionDeclaration(decl, symbolTable);
}

typedef void (*DeclTypeChecker)(CDeclaration*, SymbolTable*);

static const DeclTypeChecker declTypeCheckers[] = {
    [DECL_VAR]  = handleDeclVar,
    [DECL_FUNC] = handleDeclFunc,
};

/* ---- Block-item type-check handlers ---- */

static void handleBlockItemDecl(CBlockItem* blockItem, SymbolTable* symbolTable) {
    DeclTypeChecker handler = declTypeCheckers[blockItem->item.decl->type];
    if (!handler) return;
    handler(blockItem->item.decl, symbolTable);
}

static void handleBlockItemStmt(CBlockItem* blockItem, SymbolTable* symbolTable) {
    typeCheckStatement(blockItem->item.stmt, symbolTable);
}

typedef void (*BlockItemTypeChecker)(CBlockItem*, SymbolTable*);

static const BlockItemTypeChecker blockItemTypeCheckers[] = {
    [BLOCK_ITEM_DECL] = handleBlockItemDecl,
    [BLOCK_ITEM_STMT] = handleBlockItemStmt,
};

void typeCheckBlock(CBlock* block, SymbolTable* symbolTable) {
    if (!block) return;
    for (int i = 0; i < BlockItemArray_size(block->items); i++) {
        CBlockItem* blockItem = (CBlockItem*)BlockItemArray_get(block->items, i);
        BlockItemTypeChecker handler = blockItemTypeCheckers[blockItem->type];
        if (!handler) continue;
        handler(blockItem, symbolTable);
    }
}
