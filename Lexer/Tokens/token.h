#pragma once
#include "../../Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTExpressionTypes.h"

extern const char *tokenTypeStr[];

typedef enum {
    IDENTIFIER, 
    CONSTANT,
    INT_KEYWORD,
    VOID_KEYWORD,
    STATIC_KEYWORD,
    EXTERN_KEYWORD,
    RETURN_KEYWORD,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACE,
    CLOSE_BRACE,
    SEMICOLON,
    TILDE,
    HYPHEN,
    TWO_PLUS,
    TWO_HYPHENS,
    PLUS, 
    ASTERISK,
    SLASH,
    PERCENT,
    EXCLAMATION,
    TWO_AMPERSANDS,
    TWO_BARS,
    ONE_EQUAL,
    TWO_EQUALS,
    NOT_EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LESS_EQUAL,
    GREATER_EQUAL,
    PLUS_EQUAL,
    MINUS_EQUAL,
    STAR_EQUAL,
    SLASH_EQUAL,
    PERCENT_EQUAL,
    AMPERSAND,
    BAR,
    CARET,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    AMPERSAND_EQUAL,
    BAR_EQUAL,
    CARET_EQUAL,
    LEFT_SHIFT_EQUAL,
    RIGHT_SHIFT_EQUAL,
    IF_KEYWORD,
    ELSE_KEYWORD,
    QUESTION_MARK,
    COMMA,
    LEXER,
    WHILE_KEYWORD,
    FOR_KEYWORD,
    DO_KEYWORD,
    COLON,
    BREAK_KEYWORD,
    CONTINUE_KEYWORD,
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
char* tokenTypeToToken(TokenType type);
unaryType tokenTypeToUnaryType(TokenType type);
unaryType prefixToPostfix(unaryType type);
TokenType handleMoreThanCharOperators(char* tokenSource, char ** tokenStr);
