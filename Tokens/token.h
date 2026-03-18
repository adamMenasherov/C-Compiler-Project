#pragma once

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
    ERROR
} TokenType; 

static const char *tokenTypeStr[] = {
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
    "ERROR"
};

typedef struct {
    TokenType type;
    char* value;
} Token;


Token* createToken(char** tokenSource);
TokenType identifyType(char * tokenSource, char ** tokenStr);
TokenType keywordOrIdentifier(char * tokenSource, char ** tokenStr);
TokenType Constant(char* tokenSource, char ** tokenStr);
void freeToken(Token* tok);