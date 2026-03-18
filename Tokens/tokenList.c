#include "tokenList.h"
#include <stdlib.h>
#include <stdio.h>


void createTokenList(TokenList* tokenList) {
    tokenList->tokens = malloc(sizeof(Token) * 16);
    if (!tokenList) return;
    tokenList->arraySize = 16;
    tokenList->currSize = 0;
}


void addToken(TokenList* tokenList, Token* token) {
    if (tokenList->currSize >= tokenList->arraySize) {
        tokenList->tokens = realloc(tokenList->tokens, 
            tokenList->arraySize * 2 * sizeof(Token));
        if (!tokenList->tokens) return;
        tokenList->arraySize *= 2;
    }
    tokenList->tokens[tokenList->currSize++] = token;
}

void freeTokenList(TokenList* tokenList) {
    for (int i = 0; i < tokenList->currSize; i++) {
        freeToken(tokenList->tokens[i]);
    }
    free(tokenList->tokens);
}
void printTokenList(TokenList* tokenList) {
    for (int i = 0; i < tokenList->currSize; i++) {
        Token* tok = tokenList->tokens[i];
        printf("Token: %s   |    type: %s\n", tok->value, tokenTypeStr[tok->type]);
    }
}