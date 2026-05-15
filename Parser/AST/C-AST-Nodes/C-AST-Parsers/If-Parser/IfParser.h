#pragma once
#include "../../C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "../../../../../Lexer/Tokens/tokenList.h"

CIf* C_parseIf(TokenList* tokens);
CSwitch* C_parseSwitch(TokenList* tokens);
