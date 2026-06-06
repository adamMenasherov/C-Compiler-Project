#include "BlockParser.h"
#include "../Statement-Parser/StatementParser.h"
#include "../../TACKYUtils/TACKYEmitters.h"
#include "../../TACKYUtils/TACKYConstructors.h"
#include "../../../Common/SharedTypeRank.h"
#include "../../../../Semantic/utils/SemanticUtils/SemanticUtils.h"

typedef void (*BlockItemHandler)(CBlockItem* blockItem, TACKYInstructionList* instructionList, SymbolTable* symTable);

static void handleBlockItemDeclSingleInit(CBlockItem* blockItem, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    if (blockItem->item.decl->type == DECL_FUNC) return;
    if (blockItem->item.decl->decl.variableDecl.storageClass == STORAGE_CLASS_STATIC) return;
    char* varName = blockItem->item.decl->decl.variableDecl.identifier;
    CFactor* init = blockItem->item.decl->decl.variableDecl.init->init.singleInit;
    TACKYValue* src = emit_TACKYAndConvert(init, instructionList, symTable);
    TACKYValue* dst = createVarValue(varName);
    addInstructionToList(instructionList,
        createCopyInstruction(src, dst));
}

static void handleBlockItemCompoundInitRecurse(CInitializer* init, char* varName, int offset, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    if (init->type == INIT_SINGLE) {
        CFactor* singleInit = init->init.singleInit;
        TACKYValue* src = emit_TACKYAndConvert(singleInit, instructionList, symTable);
        TACKYValue* dst = createVarValue(varName);
        addInstructionToList(instructionList,
            createCopyToOffsetInstruction(src, dst, offset));
    } else {
        CInitializerList* initList = init->init.compoundInit.initializers;
        for (int i = 0; i < CInitializerList_size(initList); i++) {
            CInitializer* nestedInit = CInitializerList_get(initList, i);
            int nestedOffset = offset + i * size(getType(nestedInit));
            handleBlockItemCompoundInitRecurse(nestedInit, varName, nestedOffset, instructionList, symTable);
        }
    }
}

static void handleBlockItemDeclCompoundInit(CBlockItem* blockItem, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    if (blockItem->item.decl->type == DECL_FUNC) return;
    if (blockItem->item.decl->decl.variableDecl.storageClass == STORAGE_CLASS_STATIC) return;
    char* varName = blockItem->item.decl->decl.variableDecl.identifier;
    CInitializerList* initList = blockItem->item.decl->decl.variableDecl.init->init.compoundInit.initializers;
    for (int i = 0; i < CInitializerList_size(initList); i++) {
        CInitializer* init = CInitializerList_get(initList, i);
        int offset = i * size(getType(init));
        handleBlockItemCompoundInitRecurse(init, varName, offset, instructionList, symTable);
    }
}


static void handleBlockItemDecl(CBlockItem* blockItem, TACKYInstructionList* instructionList, SymbolTable* symTable) {
    CInitializer* init = blockItem->item.decl->decl.variableDecl.init;
    if (blockItem->item.decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP) {
        if (init->type == INIT_SINGLE) {
            handleBlockItemDeclSingleInit(blockItem, instructionList, symTable);
        } else {
            handleBlockItemDeclCompoundInit(blockItem, instructionList, symTable);
        }
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