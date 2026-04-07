#pragma once
#include "../../../../../Lexer/Tokens/tokenList.h"
#include "../../../../../Lexer/Tokens/token.h"
#include "../C-ASTExpressionTypes.h"

int check(TokenList* tokens, TokenType type);
int checkBinaryOp(TokenList* tokens);
int checkUnaryOp(TokenList* tokens);
int checkCompoundAssignment(TokenList* tokens);
int expectConstant(TokenList* tokens);
void expect(TokenList* tokens, TokenType type);
void expectCompoundAssignment(TokenList* tokens);
char* expectIdentifier(TokenList* tokens);
unaryType expectUnaryOp(TokenList* tokens);
int checkIncrementDecrement(TokenList* tokens);
binType expectBinaryOp(TokenList* tokens);
binType tokenTypeToBinType(TokenType type);
binType compoundAssignmentToBinType(TokenType type);
statementType loopStatementKeywordToStatementType(TokenList* tokens);
int checkIsLoopStatement(TokenList* tokens);
int checkFactorStart(TokenList* tokens);
int tokensLeft(TokenList* tokens);
int isUnaryOp(Token* tok);
int isBinaryOp(Token* tok);
int isLoopStatement(Token* tok);
int isCompoundAssignment(Token* tok);
int isPostfixUnaryOp(unaryType type);
int isIncrementDecrementOp(unaryType type);
unaryType FromPostPreFixToRegular(unaryType type);
int precedence(Token* tok);