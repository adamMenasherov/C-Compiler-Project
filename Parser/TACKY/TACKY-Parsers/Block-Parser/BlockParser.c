#include "BlockParser.h"
#include "../Statement-Parser/StatementParser.h"
#include "../../TACKYUtils/TACKYEmitters.h"
#include "../../TACKYUtils/TACKYConstructors.h"

typedef void (*BlockItemHandler)(CBlockItem* blockItem, TACKYInstructionList* instructionList, SymbolTable* symTable);

static void handleBlockItemDecl(CBlockItem* blockItem, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    if (blockItem->item.decl->type == DECL_FUNC) return;
    if (blockItem->item.decl->decl.variableDecl.storageClass == SPEC_STATIC) return;
    if (blockItem->item.decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP) {
        char* varName = blockItem->item.decl->decl.variableDecl.identifier;
        int isPostfixUnary = 0;
        TACKYValue* src = emit_TACKY(blockItem->item.decl->decl.variableDecl.exp, instructionList, &isPostfixUnary, symTable);
        TACKYValue* dst = createVarValue(varName);
        addInstructionToList(instructionList,
            createCopyInstruction(src, dst));

        if (isPostfixUnary) addInstructionToList(instructionList,
            emitUnaryPostfixInstruction(blockItem->item.decl->decl.variableDecl.exp, symTable));
    }
}

static void handleBlockItemStmt(CBlockItem* blockItem, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    parseStatementInstructions(blockItem->item.stmt, instructionList, symTable);
}

static const BlockItemHandler blockItemHandlers[] = {
    [BLOCK_ITEM_DECL] = handleBlockItemDecl,
    [BLOCK_ITEM_STMT] = handleBlockItemStmt
};

void parseBlockItemInstructions(CBlockItem* blockItem, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    if (!blockItem) return;
    blockItemHandlers[blockItem->type](blockItem, instructionList, symTable);
}

void parseBlock(CBlock* block, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    if (!block || !block->items) return;
    for (int i = 0; i < BlockItemArray_size(block->items); i++) {
        CBlockItem* elem = BlockItemArray_get(block->items, i);
        parseBlockItemInstructions(elem, instructionList, symTable);
    }
}