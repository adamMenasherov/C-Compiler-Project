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
    "TWO_PLUS",
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
    "PLUS_EQUAL",
    "MINUS_EQUAL",
    "STAR_EQUAL",
    "SLASH_EQUAL",
    "PERCENT_EQUAL",
    "AMPERSAND",
    "BAR",
    "CARET",
    "LEFT_SHIFT",
    "RIGHT_SHIFT",
    "AMPERSAND_EQUAL",
    "BAR_EQUAL",
    "CARET_EQUAL",
    "LEFT_SHIFT_EQUAL",
    "RIGHT_SHIFT_EQUAL",
    "IF_KEYWORD",
    "ELSE_KEYWORD",
    "QUESTION_MARK",
    "COLON",
    "ERROR"
};


const char *tokenTypeToSymbol[] = {
    "",
    "",
    "int",
    "void",
    "return",
    "(",
    ")",
    "{",
    "}",
    ";",
    "~",
    "-",
    "++",
    "--",
    "+",
    "*",
    "/",
    "%",
    "!",
    "&&",
    "||",
    "=",
    "==",
    "!=",
    "<",
    ">",
    "<=",
    ">=",
    "+=",
    "-=",
    "*=",
    "/=",
    "%=",
    "&",
    "|",
    "^",
    "<<",
    ">>",
    "&=",
    "|=",
    "^=",
    "<<=",
    ">>=",
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
    if ((type = handleMoreThanCharOperators(tokenSource, tokenStr)) != ERROR) return type;

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
        case '&': type = AMPERSAND; break;
        case '|': type = BAR; break;
        case '^': type = CARET; break;
        case '?': type = QUESTION_MARK; break;
        case ':': type = COLON; break;
        default: type = ERROR;
    }
    *tokenStr = strdup(tokenSource);
    *(tokenSource + 1) = c;
    return type;
}

char* tokenTypeToToken(TokenType type) {
    if (type < 0 || type >= ERROR)
        return "";
    return (char*)tokenTypeToSymbol[type];
}


TokenType handleMoreThanCharOperators(char* tokenSource, char ** tokenStr) {
    if (strncmp(tokenSource, "<<=", 3) == 0) {
        *tokenStr = strndup(tokenSource, 3);
        return LEFT_SHIFT_EQUAL;
    } 
    else if (strncmp(tokenSource, ">>=", 3) == 0) {
        *tokenStr = strndup(tokenSource, 3);
        return RIGHT_SHIFT_EQUAL;
    }    
    else if (strncmp(tokenSource, "++", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return TWO_PLUS;
    }
    else if (strncmp(tokenSource, "--", 2) == 0) {
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
    else if (strncmp(tokenSource, "+=", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return PLUS_EQUAL;
    } else if (strncmp(tokenSource, "-=", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return MINUS_EQUAL;
    } else if (strncmp(tokenSource, "*=", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return STAR_EQUAL;
    } else if (strncmp(tokenSource, "/=", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return SLASH_EQUAL;
    } else if (strncmp(tokenSource, "%=", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return PERCENT_EQUAL;
    } else if (strncmp(tokenSource, "&=", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return AMPERSAND_EQUAL;
    } else if (strncmp(tokenSource, "|=", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return BAR_EQUAL;
    } else if (strncmp(tokenSource, "^=", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return CARET_EQUAL;
    } else if (strncmp(tokenSource, "<<", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return LEFT_SHIFT;
    } else if (strncmp(tokenSource, ">>", 2) == 0) {
        *tokenStr = strndup(tokenSource, 2);
        return RIGHT_SHIFT;
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
    else if (strcmp(start, "if") == 0) type = IF_KEYWORD;
    else if (strcmp(start, "else") == 0) type = ELSE_KEYWORD;
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

unaryType tokenTypeToUnaryType(TokenType type) {
    switch(type) {
        case HYPHEN:
            return UNARY_NEGATE;
        case TILDE:
            return UNARY_COMPLEMENT;
        case EXCLAMATION:
            return UNARY_NOT;
        case TWO_PLUS:
            return UNARY_INCREMENT_PREFIX;
        case TWO_HYPHENS:
            return UNARY_DECREMENT_PREFIX;
        default:
            fprintf(stderr, "Error: Invalid unary operator token type %s\n", tokenTypeToToken(type));
            exit(1);
    }
}

unaryType prefixToPostfix(unaryType type) {
    switch(type) {
        case UNARY_INCREMENT_PREFIX: return UNARY_INCREMENT_POSTFIX;
        case UNARY_DECREMENT_PREFIX: return UNARY_DECREMENT_POSTFIX;
        default:
            fprintf(stderr, "Error: Invalid prefix unary operator type %d\n", type);
            exit(1);
    }
}