#include "tokenList.h"
#include <stdlib.h>
#include <stdio.h>


TokenList* createTokenList(void) {
    TokenList* tokenList = malloc(sizeof(TokenList));
    if (!tokenList) return NULL;
    
    tokenList->array = TokenArray_create();
    if (!tokenList->array) {
        free(tokenList);
        return NULL;
    }
    
    return tokenList;
}


void addToken(TokenList* tokenList, Token* token) {
    if (!tokenList || !tokenList->array) return;
    TokenArray_append(tokenList->array, token);
}

void freeTokenList(TokenList* tokenList) {
    if (!tokenList) return;
    if (tokenList->array) {
        TokenArray_freeWithTokens(tokenList->array);
    }
    free(tokenList);
}