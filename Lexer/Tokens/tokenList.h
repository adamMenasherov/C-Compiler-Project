#pragma once
#include "token.h"
#include "../../DataStructures/DynamicArray/TokenArrayWrapper.h"

typedef struct {
    TokenArray* array;  
} TokenList;

// createTokenList now allocates and returns a TokenList pointer
TokenList* createTokenList(void);
void addToken(TokenList* tokenList, Token* token);
void freeTokenList(TokenList* tokenList);
void printTokenList(TokenList* tokenList);

// Inline helper macros for backward compatibility with existing code
static inline int TokenList_size(TokenList* tl) {
    return tl && tl->array ? TokenArray_size(tl->array) : 0;
}

static inline int TokenList_getCursor(TokenList* tl) {
    return tl && tl->array ? TokenArray_getCursor(tl->array) : 0;
}

static inline void TokenList_setCursor(TokenList* tl, int pos) {
    if (tl && tl->array) TokenArray_setCursor(tl->array, pos);
}

static inline Token* TokenList_getAt(TokenList* tl, int index) {
    return tl && tl->array ? TokenArray_get(tl->array, index) : NULL;
}