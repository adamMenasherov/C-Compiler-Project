#include "LoopLabeler.h"
#include "../StatementLabeler/StatementLabeler.h"
#include "../../../../DataStructures/DynamicArray/DynamicArray.h"
#include <stdlib.h>

static void handleBlockItemDecl(CBlockItem* block) {}

static void handleBlockItemStmt(CBlockItem* block) {
    labelStatement(block->item.stmt, NULL);
}

typedef void (*BlockItemLabeler)(CBlockItem*);

static const BlockItemLabeler blockItemLabelers[] = {
    [BLOCK_ITEM_DECL] = handleBlockItemDecl,
    [BLOCK_ITEM_STMT] = handleBlockItemStmt,
};

void resolveBlockWithLabeling(CBlock* block) {
    if (!block) return;
    DArray_forEach(block->items, elem,
    {
        CBlockItem* blockItem = (CBlockItem*)elem;
        resolveBlockStatementsWithLabeling(blockItem);
    });
}

void resolveBlockStatementsWithLabeling(CBlockItem* block) {
    BlockItemLabeler handler = blockItemLabelers[block->type];
    if (!handler) return;
    handler(block);
}
