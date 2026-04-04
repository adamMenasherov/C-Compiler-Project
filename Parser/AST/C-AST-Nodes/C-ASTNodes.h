#pragma once
#include "../../../Lexer/Tokens/tokenList.h"
#include "../../../Lexer/Tokens/token.h"
#include "C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "C-ASTNodeUtilities/TokenExpect/C-ASTNodeExpect.h"
#include "../../../DataStructures/DynamicArray/BlockItemArrayWrapper.h"

// CBlockItemList now uses the generic BlockItemArray internally
// This typedef replaces the old struct-based implementation
typedef BlockItemArray CBlockItemList;


typedef struct {
    char* function_name; 
    CBlockItemList* body;  // body is a single return statement for now, not a block of items
} CFunction;

typedef struct {
    CFunction* function_def;
} CProgram;


CConstant* C_parseConstant(TokenList* tokens);
CVar* C_parseVar(TokenList* tokens);
CStatement* C_parseStatement(TokenList* tokens);
CDecleration* C_parseDecleration(TokenList* tokens);
CBlockItem* C_parseBlockItem(TokenList* tokens);
CReturn* C_parseReturn(TokenList* tokens);
CFunction* C_parseFunction(TokenList* tokens);
CProgram* C_parseProgram(TokenList* tokens);
CFactor* C_parseFactor(TokenList* tokens);
CFactor* C_parseExpression(TokenList* tokens, int min_prec);


// CBlockItemList functions
static inline CBlockItemList* createCBlockItemList() {
    return BlockItemArray_create();
}
static inline void addCBlockItem(CBlockItemList* list, CBlockItem* item) {
    BlockItemArray_add(list, item);
}

