#pragma once
#include "../../../../../Lexer/Tokens/tokenList.h"
#include "../../../../../Lexer/Tokens/token.h"
#include "../C-ASTExpressionTypes.h"
#include <stdint.h>

int check(TokenList* tokens, TokenType type);
int checkBinaryOp(TokenList* tokens);
int checkUnaryOp(TokenList* tokens);
int checkConstant(TokenList* tokens);
int checkCompoundAssignment(TokenList* tokens);
void expectConstant(TokenList* tokens, int* type, uint64_t* intValue, double* floatValue);
int lookAheadOne(TokenList* tokens, TokenType type);
int lookAheadOneType(TokenList* tokens);
void expect(TokenList* tokens, TokenType type);
void expectCompoundAssignment(TokenList* tokens);
char* expectIdentifier(TokenList* tokens);
unaryType expectUnaryOp(TokenList* tokens);
int checkIncrementDecrement(TokenList* tokens);
int checkSpecifier(TokenList* tokens);
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
constantType determineConstantType(uint64_t val, constantType type);
int isCompoundAssignment(Token* tok);
int isPostfixUnaryOp(unaryType type);
int isIncrementDecrementOp(unaryType type);
unaryType FromPostPreFixToRegular(unaryType type);
int precedence(Token* tok);
specifierType getTypeFromCFuncType(CFuncType* type);