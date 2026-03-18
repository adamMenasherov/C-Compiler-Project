#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


Token* createToken(char** tokenSource) {
    while(isspace(**tokenSource)) (*tokenSource)++;
    if (**tokenSource == '\0') return NULL; // No more tokens

    Token* newTok = malloc(sizeof(Token));
    char* tokenStr = NULL;
    newTok->type = identifyType(*tokenSource, &tokenStr);
    newTok->value = tokenStr;

    return newTok;
}

TokenType identifyType(char * tokenSource, char ** tokenStr) {
    TokenType type;
    char c;
    if (isalpha(*tokenSource)) return keywordOrIdentifier(tokenSource, tokenStr);
    if (isdigit(*tokenSource)) return Constant(tokenSource, tokenStr);

    c = *(tokenSource + 1);
    // Has to be one of the single char tokens, so we terminate the string after the first char
    *(tokenSource + 1) = '\0';
    switch (*tokenSource) {
        case '(': type = OPEN_PAREN; break;
        case ')': type = CLOSE_PAREN; break;
        case '{': type = OPEN_BRACE; break;
        case '}': type = CLOSE_BRACE; break;
        case ';': type = SEMICOLON; break;
        default: type = IDENTIFIER;
    }
    *tokenStr = strdup(tokenSource);
    *(tokenSource + 1) = c;
    return type;
}


TokenType keywordOrIdentifier(char* tokenSource, char ** tokenStr) {
    TokenType type;
    char* start = tokenSource, c;
    while (isalnum(*tokenSource)) tokenSource++;

    c = *tokenSource;
    *tokenSource = '\0';
    
    if (strcmp(start, "int") == 0) type = INT_KEYWORD;
    else if (strcmp(start, "void") == 0) type = VOID_KEYWORD;
    else if (strcmp(start, "return") == 0) type = RETURN_KEYWORD;
    else type = IDENTIFIER;
    
    *tokenStr = strdup(start);
    *tokenSource = c;

    return type;
}

TokenType Constant(char* tokenSource, char ** tokenStr) {
    char* start = tokenSource;
    while (isdigit(*tokenSource)) tokenSource++;
    
    char c = *tokenSource;
    *tokenSource = '\0';
    *tokenStr = strdup(start);
    *tokenSource = c;

    return CONSTANT;
}

void freeToken(Token* tok) {
    free(tok->value);
    free(tok);
}

