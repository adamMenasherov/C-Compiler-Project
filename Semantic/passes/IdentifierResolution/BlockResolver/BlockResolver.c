#include "BlockResolver.h"
#include "../IdentifierResolver/IdentifierResolver.h"
#include "../StatementResolver/StatementResolver.h"
#include "../../../../DataStructures/DynamicArray/Wrappers/BlockItemArrayWrapper.h"
#include <stdlib.h>

static void handleBlockItemDecl(CBlockItem* blockItem, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveDeclaration(blockItem->item.decl, varMap, symbolTable, BLOCK_LEVEL);
}

static void handleBlockItemStmt(CBlockItem* blockItem, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveStatement(blockItem->item.stmt, varMap, symbolTable);
}

typedef void (*BlockItemHandler)(CBlockItem*, SemanticIdentifierMap*, SymbolTable*);

static const BlockItemHandler blockItemHandlers[] = {
    [BLOCK_ITEM_DECL] = handleBlockItemDecl,
    [BLOCK_ITEM_STMT] = handleBlockItemStmt,
};

void resolveBlock(CBlock* block, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!block) return;
    for (int i = 0; i < BlockItemArray_size(block->items); i++) {
        CBlockItem* blockItem = (CBlockItem*)BlockItemArray_get(block->items, i);
        resolveBlockItem(blockItem, varMap, symbolTable);
    }
}

void resolveBlockItem(CBlockItem* blockItem, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!blockItem) return;
    BlockItemHandler handler = blockItemHandlers[blockItem->type];
    if (!handler) return;
    handler(blockItem, varMap, symbolTable);
}
