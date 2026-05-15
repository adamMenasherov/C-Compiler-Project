#pragma once
#include "../../C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "../../../../../Lexer/Tokens/tokenList.h"

CLoop* C_parseWhile(TokenList* tokens);
CLoop* C_parseDoWhile(TokenList* tokens);
CForLoop* C_parseFor(TokenList* tokens);
CForInit* C_parseForInit(TokenList* tokens);
CStatement* C_parseLoopStatement(TokenList* tokens);