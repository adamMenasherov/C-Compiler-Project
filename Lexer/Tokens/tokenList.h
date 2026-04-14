#pragma once
#include "token.h"
#include "../../DataStructures/DynamicArray/Wrappers/TokenArrayWrapper.h"

typedef struct {
    TokenArray* array;  
} TokenList;

// createTokenList now allocates and returns a TokenList pointer
TokenList* createTokenList(void);
void addToken(TokenList* tokenList, Token* token);
void freeTokenList(TokenList* tokenList);
void printTokenList(TokenList* tokenList);