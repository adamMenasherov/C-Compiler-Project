#pragma once
#include "../../../../../Lexer/Tokens/tokenList.h"
#include "../../../../../Lexer/Tokens/token.h"
#include "../C-ASTExpressionTypes.h"

int check(TokenList* tokens, TokenType type);
int checkBinaryOp(TokenList* tokens);
int checkUnaryOp(TokenList* tokens);
int expectConstant(TokenList* tokens);
void expect(TokenList* tokens, TokenType type);
char* expectIdentifier(TokenList* tokens);
unaryType expectUnaryOp(TokenList* tokens);
binType expectBinaryOp(TokenList* tokens);
binType tokenTypeToBinType(TokenType type);
int checkFactorStart(TokenList* tokens);
int tokensLeft(TokenList* tokens);
int isUnaryOp(Token* tok);
int isBinaryOp(Token* tok);
int precedence(Token* tok);