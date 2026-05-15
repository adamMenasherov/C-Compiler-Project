#include "TokenPrint.h"
#include <stdio.h>

void printTokenList(TokenList* tokenList) {
    if (!tokenList || !tokenList->array) return;
    
    for (int i = 0; i < TokenArray_size(tokenList->array); i++) {
        Token* tok = TokenArray_get(tokenList->array, i);
        printf("Token: %s   |    type: %s\n", tok->value, tokenTypeToToken(tok->type));
    }
}