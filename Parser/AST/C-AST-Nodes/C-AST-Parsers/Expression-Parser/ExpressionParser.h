#pragma once
#include "../../C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "../../../../../Lexer/Tokens/tokenList.h"


CFactor* C_parseFactor(TokenList* tokens);
CFactor* C_parseExpression(TokenList* tokens, int min_prec);
CFactor* C_parseConditionalMiddle(TokenList* tokens);
CConstant* C_parseConstant(TokenList* tokens);
CVar* C_parseVar(TokenList* tokens);
CCast* C_parseCast(TokenList* tokens);
ExpressionFactorArray* parseArgumentList(TokenList* tokens);