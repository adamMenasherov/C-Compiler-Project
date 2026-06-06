#pragma once
#include "token.h"
#include "../../DataStructures/DynamicArray/Wrappers/TokenArrayWrapper.h"

/**
 * @brief The TokenList. 
 * TokenList is a wrapper around a dynamic array of tokens, 
 * providing additional functionality specific to handling lists of tokens.
 */
typedef struct {
    TokenArray* array;  
} TokenList;

/**
 * @brief Create a Token List object
 * 
 * @return TokenList* The created Token List object, or NULL if memory allocation fails.
 */
TokenList* createTokenList(void);
/**
 * @brief Add a token to the token list.
 * 
 * @param tokenList The token list to add the token to.
 * @param token The token to add to the token list.
 */
void addToken(TokenList* tokenList, Token* token);
/**
 * @brief Free the memory allocated for the token list and its tokens.
 * 
 * @param tokenList The token list to free.
 */
void freeTokenList(TokenList* tokenList);
/**
 * @brief Print the tokens in the token list for debugging purposes.
 * 
 * @param tokenList The token list to print.
 */
void printTokenList(TokenList* tokenList);