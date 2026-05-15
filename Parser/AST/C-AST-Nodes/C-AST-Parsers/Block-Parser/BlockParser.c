#include "BlockParser.h"
#include "../C-ParsersInclude.h"

CBlock* C_parseBlock(TokenList* tokens) {
    CBlock* block = C_CreateBlockEmpty();
    if (!block) return NULL;
    while (!check(tokens, CLOSE_BRACE)) {
        CBlockItem* item = C_parseBlockItem(tokens);
        if (!item) return NULL;
        BlockItemArray_append(block->items, item);
    }
    return block;
}

CBlockItem* C_parseBlockItem(TokenList* tokens) {
    blockItemType type;
    void* data;
    if (checkSpecifier(tokens)) {
        type = BLOCK_ITEM_DECL;
        CDeclaration* decl = C_parseDeclaration(tokens);
        data = decl;
    }    
    else {
        type = BLOCK_ITEM_STMT;
        CStatement* stmt = C_parseStatement(tokens);
        data = stmt;
    }  

    return C_CreateBlockItem(type, data);
}