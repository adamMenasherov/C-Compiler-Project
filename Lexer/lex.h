#pragma once
#include "Tokens/tokenList.h"

/**
 * @brief The main lexing function. 
 * It takes a pointer to the source code and a token list to populate with the tokens found in the source code.
 * 
 * @param source A pointer to the source code to be lexed.
 * @param tokenList The token list to populate with the tokens found in the source code.
 */
void lex(char ** source, TokenList* tokenList);