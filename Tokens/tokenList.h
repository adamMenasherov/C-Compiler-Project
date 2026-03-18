#pragma once
#include "token.h"

typedef struct {
    Token** tokens;
    int currSize;
    int arraySize;
} TokenList;

void createTokenList(TokenList* tokenList);
void addToken(TokenList* tokenList, Token* token);
void freeTokenList(TokenList* tokenList);
void printTokenList(TokenList* tokenList);