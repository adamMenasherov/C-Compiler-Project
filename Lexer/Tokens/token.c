#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


const char *tokenTypeStr[] = {
    "IDENTIFIER", 
    "CONSTANT",
    "INT_KEYWORD",
    "VOID_KEYWORD",
    "RETURN_KEYWORD",
    "OPEN_PAREN",
    "CLOSE_PAREN",
    "OPEN_BRACE",
    "CLOSE_BRACE",
    "SEMICOLON",
    "TILDE",
    "HYPHEN",
    "TWO_HYPHENS",
    "PLUS", 
    "ASTERISK",
    "SLASH",
    "PERCENT",
    "EXCLAMATION",
    "TWO_AMPERSANDS",
    "TWO_BARS",
    "ONE_EQUAL",
    "TWO_EQUALS",
    "NOT_EQUAL",
    "LESS_THAN",
    "GREATER_THAN",
    "LESS_EQUAL",
    "GREATER_EQUAL",
    "ERROR"
};



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
    if ((type = handleTwoCharOperators(tokenSource, tokenStr)) != ERROR) return type;

    c = *(tokenSource + 1);
    // Has to be one of the single char tokens, so we terminate the string after the first char
    *(tokenSource + 1) = '\0';
    switch (*tokenSource) {
        case '=': type = ONE_EQUAL; break;
        case '(': type = OPEN_PAREN; break;
        case ')': type = CLOSE_PAREN; break;
        case '{': type = OPEN_BRACE; break;
        case '}': type = CLOSE_BRACE; break;
        case ';': type = SEMICOLON; break;
        case '~': type = TILDE; break;
        case '-': type = HYPHEN; break;
        case '+': type = PLUS; break;
        case '*': type = ASTERISK; break;
        case '/': type = SLASH; break;
        case '%': type = PERCENT; break;
        case '!': type = EXCLAMATION; break;
        case '<': type = LESS_THAN; break;
        case '>': type = GREATER_THAN; break;
        default: type = ERROR;
    }
    *tokenStr = strdup(tokenSource);
    *(tokenSource + 1) = c;
    return type;
}

TokenType handleTwoCharOperators(char* tokenSource, char ** tokenStr) {
    if (strncmp(tokenSource, "--", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return TWO_HYPHENS;
    }
    else if (strncmp(tokenSource, "&&", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return TWO_AMPERSANDS;
    } else if (strncmp(tokenSource, "||", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return TWO_BARS;
    } else if (strncmp(tokenSource, "==", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return TWO_EQUALS;
    } else if (strncmp(tokenSource, "!=", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return NOT_EQUAL;
    } else if (strncmp(tokenSource, "<=", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return LESS_EQUAL;
    } else if (strncmp(tokenSource, ">=", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return GREATER_EQUAL;
    }

    return ERROR;
}


TokenType keywordOrIdentifier(char* tokenSource, char ** tokenStr) {
    TokenType type;
    char* start = tokenSource, c;
    tokenSource++;
    while (isalnum(*tokenSource) || *tokenSource == '_') tokenSource++;

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
    if (!tok) return;
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
        default:
            break;
    }

    return "";
}


unaryType tokenTypeToUnaryType(TokenType type) {
    switch(type) {
        case HYPHEN:
            return UNARY_NEGATE;
        case TILDE:
            return UNARY_COMPLEMENT;
        case EXCLAMATION:
            return UNARY_NOT;
        default:
            fprintf(stderr, "Error: Invalid unary operator token type %s\n", tokenTypetoToken(type));
            exit(1);
    }
}