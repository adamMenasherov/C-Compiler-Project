#pragma once
#include "../../Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTExpressionTypes.h"

extern const char *tokenTypeStr[];

typedef enum {
    IDENTIFIER, 
    CONSTANT,
    INT_KEYWORD,
    VOID_KEYWORD,
    RETURN_KEYWORD,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACE,
    CLOSE_BRACE,
    SEMICOLON,
    TILDE,
    HYPHEN,
    TWO_HYPHENS,
    PLUS, 
    ASTERISK,
    SLASH,
    PERCENT,
    ERROR
} TokenType; 

typedef struct {
    TokenType type;
    char* value;
} Token;


Token* createToken(char** tokenSource);
TokenType identifyType(char * tokenSource, char ** tokenStr);
TokenType keywordOrIdentifier(char * tokenSource, char ** tokenStr);
TokenType isConstant(char* tokenSource, char ** tokenStr);
void freeToken(Token* tok);
char* tokenTypetoToken(TokenType type);
unaryType tokenTypeToUnaryType(TokenType type);
