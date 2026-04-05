#pragma once
#include "../../../Lexer/Tokens/tokenList.h"
#include "../../../Lexer/Tokens/token.h"
#include "C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "C-ASTNodeUtilities/TokenExpect/C-ASTNodeExpect.h"


typedef struct {
    char* function_name; 
    CBlock* block;  // body is a single return statement for now, not a block of items
} CFunction;

typedef struct {
    CFunction* function_def;
} CProgram;


CConstant* C_parseConstant(TokenList* tokens);
CVar* C_parseVar(TokenList* tokens);
CIf* C_parseIf(TokenList* tokens);
CStatement* C_parseStatement(TokenList* tokens);
CDeclaration* C_parseDecleration(TokenList* tokens);
CBlockItem* C_parseBlockItem(TokenList* tokens);
CReturn* C_parseReturn(TokenList* tokens);
CFunction* C_parseFunction(TokenList* tokens);
CProgram* C_parseProgram(TokenList* tokens);
CFactor* C_parseFactor(TokenList* tokens);
CFactor* C_parseExpression(TokenList* tokens, int min_prec);
CFactor* C_parseConditionalMiddle(TokenList* tokens);
CBlock* C_parseBlock(TokenList* tokens);


// CBlockItemList functions
static inline CBlockItemList* createCBlockItemList() {
    return BlockItemArray_create();
}
static inline void addCBlockItem(CBlockItemList* list, CBlockItem* item) {
    BlockItemArray_append(list, item);
}

