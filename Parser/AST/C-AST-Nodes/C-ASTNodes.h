#pragma once
#include "../../../Lexer/Tokens/tokenList.h"
#include "../../../Lexer/Tokens/token.h"
#include "C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "C-ASTNodeUtilities/TokenExpect/C-ASTNodeExpect.h"


typedef struct {
    CBlockItem** items;
    int currSize;
    int arraySize;
    int cursor; 
} CBlockItemList;


typedef struct {
    char* function_name; 
    CBlockItemList* body; 
} CFunction;

typedef struct {
    CFunction* function_def;
} CProgram;


CConstant* C_parseConstant(TokenList* tokens);
CReturn* C_parseReturn(TokenList* tokens);
CFunction* C_parseFunction(TokenList* tokens);
CProgram* C_parseProgram(TokenList* tokens);
CFactor* C_parseFactor(TokenList* tokens);
CFactor* C_parseExpression(TokenList* tokens, int min_prec);

