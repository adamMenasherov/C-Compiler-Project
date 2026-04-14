#pragma once
#include "../../../Lexer/Tokens/tokenList.h"
#include "../../../Lexer/Tokens/token.h"
#include "C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "C-ASTNodeUtilities/TokenExpect/C-ASTNodeExpect.h"
#include "../../../DataStructures/DynamicArray/Wrappers/CDeclarationWrapper.h"


typedef struct {
    CDeclarationArray* function_def;
} CProgram;


CConstant* C_parseConstant(TokenList* tokens);
CVar* C_parseVar(TokenList* tokens);
CIf* C_parseIf(TokenList* tokens);
CLoop* C_parseWhile(TokenList* tokens);
CLoop* C_parseDoWhile(TokenList* tokens);
CForLoop* C_parseFor(TokenList* tokens);
CForInit* C_parseForInit(TokenList* tokens);
CStatement* C_parseLoopStatement(TokenList* tokens);
CStatement* C_parseStatement(TokenList* tokens);
CDeclaration* C_parseVarDeclaration(TokenList* tokens, char* identifier);
CDeclaration* C_parseDeclaration(TokenList* tokens);
CBlockItem* C_parseBlockItem(TokenList* tokens);
CReturn* C_parseReturn(TokenList* tokens);
IdentifierArray* C_parseFuncParameters(TokenList* tokens);
CDeclaration* C_parseFunction(TokenList* tokens, char* identifier);
CDeclarationArray* C_parseFunctions(TokenList* tokens);
CProgram* C_parseProgram(TokenList* tokens);
CFactor* C_parseFactor(TokenList* tokens);
CFactor* C_parseExpression(TokenList* tokens, int min_prec);
CFactor* C_parseConditionalMiddle(TokenList* tokens);
CBlock* C_parseBlock(TokenList* tokens);
ExpressionFactorArray* parseArgumentList(TokenList* tokens);
