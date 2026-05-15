#pragma once
#include "../../C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "../../../../../Lexer/Tokens/tokenList.h"

CBlockItem* C_parseBlockItem(TokenList* tokens);
CBlock* C_parseBlock(TokenList* tokens);