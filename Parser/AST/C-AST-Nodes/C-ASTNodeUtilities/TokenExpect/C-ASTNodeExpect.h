#pragma once
#include "../../../../../Lexer/Tokens/tokenList.h"
#include "../../../../../Lexer/Tokens/token.h"
#include "../C-ASTExpressionTypes.h"

int check(TokenList* tokens, TokenType type);
int expectConstant(TokenList* tokens);
void expect(TokenList* tokens, TokenType type);
char* expectIdentifier(TokenList* tokens);
unaryType expectUnaryOp(TokenList* tokens);
int tokensLeft(TokenList* tokens);
int isUnaryOp(Token* tok);
