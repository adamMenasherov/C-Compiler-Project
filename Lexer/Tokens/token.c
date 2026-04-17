#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


const char *tokenTypeStr[] = {
    [IDENTIFIER] = "IDENTIFIER",
    [CONSTANT] = "CONSTANT",
    [INT_KEYWORD] = "INT_KEYWORD",
    [VOID_KEYWORD] = "VOID_KEYWORD",
    [STATIC_KEYWORD] = "STATIC_KEYWORD",
    [EXTERN_KEYWORD] = "EXTERN_KEYWORD",
    [RETURN_KEYWORD] = "RETURN_KEYWORD",
    [OPEN_PAREN] = "OPEN_PAREN",
    [CLOSE_PAREN] = "CLOSE_PAREN",
    [OPEN_BRACE] = "OPEN_BRACE",
    [CLOSE_BRACE] = "CLOSE_BRACE",
    [SEMICOLON] = "SEMICOLON",
    [TILDE] = "TILDE",
    [HYPHEN] = "HYPHEN",
    [TWO_PLUS] = "TWO_PLUS",
    [TWO_HYPHENS] = "TWO_HYPHENS",
    [PLUS] = "PLUS",
    [ASTERISK] = "ASTERISK",
    [SLASH] = "SLASH",
    [PERCENT] = "PERCENT",
    [EXCLAMATION] = "EXCLAMATION",
    [TWO_AMPERSANDS] = "TWO_AMPERSANDS",
    [TWO_BARS] = "TWO_BARS",
    [ONE_EQUAL] = "ONE_EQUAL",
    [TWO_EQUALS] = "TWO_EQUALS",
    [NOT_EQUAL] = "NOT_EQUAL",
    [LESS_THAN] = "LESS_THAN",
    [GREATER_THAN] = "GREATER_THAN",
    [LESS_EQUAL] = "LESS_EQUAL",
    [GREATER_EQUAL] = "GREATER_EQUAL",
    [PLUS_EQUAL] = "PLUS_EQUAL",
    [MINUS_EQUAL] = "MINUS_EQUAL",
    [STAR_EQUAL] = "STAR_EQUAL",
    [SLASH_EQUAL] = "SLASH_EQUAL",
    [PERCENT_EQUAL] = "PERCENT_EQUAL",
    [AMPERSAND] = "AMPERSAND",
    [BAR] = "BAR",
    [CARET] = "CARET",
    [LEFT_SHIFT] = "LEFT_SHIFT",
    [RIGHT_SHIFT] = "RIGHT_SHIFT",
    [AMPERSAND_EQUAL] = "AMPERSAND_EQUAL",
    [BAR_EQUAL] = "BAR_EQUAL",
    [CARET_EQUAL] = "CARET_EQUAL",
    [LEFT_SHIFT_EQUAL] = "LEFT_SHIFT_EQUAL",
    [RIGHT_SHIFT_EQUAL] = "RIGHT_SHIFT_EQUAL",
    [IF_KEYWORD] = "IF_KEYWORD",
    [ELSE_KEYWORD] = "ELSE_KEYWORD",
    [QUESTION_MARK] = "QUESTION_MARK",
    [COMMA] = "COMMA",
    [COLON] = "COLON",
    [WHILE_KEYWORD] = "WHILE_KEYWORD",
    [FOR_KEYWORD] = "FOR_KEYWORD",
    [DO_KEYWORD] = "DO_KEYWORD",
    [BREAK_KEYWORD] = "BREAK_KEYWORD",
    [CONTINUE_KEYWORD] = "CONTINUE_KEYWORD",
    [ERROR] = "ERROR"
};


const char *tokenTypeToSymbol[] = {
    [IDENTIFIER] = "",
    [CONSTANT] = "",
    [INT_KEYWORD] = "int",
    [VOID_KEYWORD] = "void",
    [STATIC_KEYWORD] = "static",
    [EXTERN_KEYWORD] = "extern",
    [RETURN_KEYWORD] = "return",
    [OPEN_PAREN] = "(",
    [CLOSE_PAREN] = ")",
    [OPEN_BRACE] = "{",
    [CLOSE_BRACE] = "}",
    [SEMICOLON] = ";",
    [TILDE] = "~",
    [HYPHEN] = "-",
    [TWO_PLUS] = "++",
    [TWO_HYPHENS] = "--",
    [PLUS] = "+",
    [ASTERISK] = "*",
    [SLASH] = "/",
    [PERCENT] = "%",
    [EXCLAMATION] = "!",
    [TWO_AMPERSANDS] = "&&",
    [TWO_BARS] = "||",
    [ONE_EQUAL] = "=",
    [TWO_EQUALS] = "==",
    [NOT_EQUAL] = "!=",
    [LESS_THAN] = "<",
    [GREATER_THAN] = ">",
    [LESS_EQUAL] = "<=",
    [GREATER_EQUAL] = ">=",
    [PLUS_EQUAL] = "+=",
    [MINUS_EQUAL] = "-=",
    [STAR_EQUAL] = "*=",
    [SLASH_EQUAL] = "/=",
    [PERCENT_EQUAL] = "%=",
    [AMPERSAND] = "&",
    [BAR] = "|",
    [CARET] = "^",
    [LEFT_SHIFT] = "<<",
    [RIGHT_SHIFT] = ">>",
    [AMPERSAND_EQUAL] = "&=",
    [BAR_EQUAL] = "|=",
    [CARET_EQUAL] = "^=",
    [LEFT_SHIFT_EQUAL] = "<<=",
    [RIGHT_SHIFT_EQUAL] = ">>=",
    [IF_KEYWORD] = "if",
    [ELSE_KEYWORD] = "else",
    [QUESTION_MARK] = "?",
    [COMMA] = ",",
    [COLON] = ":",
    [WHILE_KEYWORD] = "while",
    [FOR_KEYWORD] = "for",
    [DO_KEYWORD] = "do",
    [BREAK_KEYWORD] = "break",
    [CONTINUE_KEYWORD] = "continue",
    [ERROR] = ""
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
        case ',': type = COMMA; break;
        case ':': type = COLON; break;
        default: type = ERROR;
    }
    *tokenStr = strdup(tokenSource);
    *(tokenSource + 1) = c;
    return type;
}

char* tokenTypeToToken(TokenType type) {
    if (type < 0 || type > ERROR)
        return "";
    return (char*)(tokenTypeToSymbol[type] ? tokenTypeToSymbol[type] : "");
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
    else if (strcmp(start, "static") == 0) type = STATIC_KEYWORD;
    else if (strcmp(start, "extern") == 0) type = EXTERN_KEYWORD;
    else if (strcmp(start, "return") == 0) type = RETURN_KEYWORD;
    else if (strcmp(start, "if") == 0) type = IF_KEYWORD;
    else if (strcmp(start, "else") == 0) type = ELSE_KEYWORD;
    else if (strcmp(start, "while") == 0) type = WHILE_KEYWORD;
    else if (strcmp(start, "for") == 0) type = FOR_KEYWORD;
    else if (strcmp(start, "do") == 0) type = DO_KEYWORD;
    else if (strcmp(start, "break") == 0) type = BREAK_KEYWORD;
    else if (strcmp(start, "continue") == 0) type = CONTINUE_KEYWORD;
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