#pragma once
#include "../../C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "../../../../../Lexer/Tokens/tokenList.h"

CStatement* C_parseStatement(TokenList* tokens);
CReturn* C_parseReturn(TokenList* tokens);