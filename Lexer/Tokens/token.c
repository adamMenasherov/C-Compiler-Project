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
    if (isalpha(*tokenSource) || *tokenSource == '_') return keywordOrIdentifier(tokenSource, tokenStr);
    if (isdigit(*tokenSource)) return isConstant(tokenSource, tokenStr);
    if (strncmp(tokenSource, "--", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return TWO_HYPHENS;
    }

    c = *(tokenSource + 1);
    // Has to be one of the single char tokens, so we terminate the string after the first char
    *(tokenSource + 1) = '\0';
    switch (*tokenSource) {
        case '(': type = OPEN_PAREN; break;
        case ')': type = CLOSE_PAREN; break;
        case '{': type = OPEN_BRACE; break;
        case '}': type = CLOSE_BRACE; break;
        case ';': type = SEMICOLON; break;
        case '~': type = TILDE; break;
        case '-': type = HYPHEN; break;
        default: type = ERROR;
    }
    *tokenStr = strdup(tokenSource);
    *(tokenSource + 1) = c;
    return type;
}


TokenType keywordOrIdentifier(char* tokenSource, char ** tokenStr) {
    TokenType type;
    char* start = tokenSource, c;
    tokenSource++;
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

TokenType isConstant(char* tokenSource, char ** tokenStr) {
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


char* tokenTypetoToken(TokenType type) {
    switch(type) {
        case INT_KEYWORD:
            return "int";
        case VOID_KEYWORD:
            return "void";
        case RETURN_KEYWORD:
            return "return";
        case OPEN_PAREN:
            return "(";
        case CLOSE_PAREN:
            return ")";
        case OPEN_BRACE:
            return "{";
        case CLOSE_BRACE:
            return "}";
        case SEMICOLON:
            return ";";
    }

    return "";
}


unaryType tokenTypeToUnaryType(TokenType type) {
    switch(type) {
        case HYPHEN:
            return UNARY_NEGATE;
        case TILDE:
            return UNARY_COMPLEMENT;
        default:
            fprintf(stderr, "Error: Invalid unary operator token type %s\n", tokenTypetoToken(type));
            exit(1);
    }
}