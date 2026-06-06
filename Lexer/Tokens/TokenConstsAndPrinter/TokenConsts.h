#pragma once
#include "../../../Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTExpressionTypes.h"

extern const char *tokenTypeStr[];
extern const char *tokenTypeToSymbol[];

// All the token types the lexer can produce. 
typedef enum {
    IDENTIFIER, 
    CONSTANT,
    LONG_CONSTANT,
    UNSIGNED_CONSTANT,
    UNSIGNED_LONG_CONSTANT,
    FLOATING_POINT_CONSTANT,
    INT_KEYWORD,
    LONG_KEYWORD,
    VOID_KEYWORD,
    STATIC_KEYWORD,
    EXTERN_KEYWORD,
    RETURN_KEYWORD,
    SWITCH_KEYWORD,
    DOUBLE_KEYWORD,
    DEFAULT_KEYWORD,
    CASE_KEYWORD,
    OPEN_PAREN,
    CLOSE_PAREN,
    AMPERSAND,
    OPEN_BRACE,
    CLOSE_BRACE,
    SEMICOLON,
    TILDE,
    HYPHEN,
    TWO_PLUS,
    TWO_HYPHENS,
    PLUS, 
    ASTERISK,
    OPEN_BRACKET,
    CLOSE_BRACKET,
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
    BAR,
    CARET,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    SIGNED,
    UNSIGNED,
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
    ERROR,
    NOT_ACCEPTING
} TokenType;

// All the keyword tokens the lexer can produce.
typedef enum {
    KEYWORD_TOKEN_INT,
    KEYWORD_TOKEN_LONG,
    KEYWORD_TOKEN_VOID,
    KEYWORD_TOKEN_STATIC,
    KEYWORD_TOKEN_DEFAULT,
    KEYWORD_TOKEN_CASE,
    KEYWORD_TOKEN_EXTERN,
    KEYWORD_TOKEN_RETURN,
    KEYWORD_TOKEN_SWITCH,
    KEYWORD_TOKEN_IF,
    KEYWORD_TOKEN_ELSE,
    KEYWORD_TOKEN_DOUBLE,
    KEYWORD_TOKEN_SIGNED,
    KEYWORD_TOKEN_UNSIGNED,
    KEYWORD_TOKEN_WHILE,
    KEYWORD_TOKEN_FOR,
    KEYWORD_TOKEN_DO,
    KEYWORD_TOKEN_BREAK,
    KEYWORD_TOKEN_CONTINUE,
    KEYWORD_TOKEN_END_OF_KEYWORDS
} KeywordTokenType;

// All the single character tokens the lexer can produce.
typedef enum {
    SINGLE_CHAR_TOKEN_OPEN_PAREN,
    SINGLE_CHAR_TOKEN_CLOSE_PAREN,
    SINGLE_CHAR_TOKEN_OPEN_BRACE,
    SINGLE_CHAR_TOKEN_CLOSE_BRACE,
    SINGLE_CHAR_TOKEN_SEMICOLON,
    SINGLE_CHAR_TOKEN_TILDE,
    SINGLE_CHAR_TOKEN_HYPHEN,
    SINGLE_CHAR_TOKEN_OPEN_BRACKET,
    SINGLE_CHAR_TOKEN_CLOSE_BRACKET,
    SINGLE_CHAR_TOKEN_PLUS,
    SINGLE_CHAR_TOKEN_ASTERISK,
    SINGLE_CHAR_TOKEN_SLASH,
    SINGLE_CHAR_TOKEN_PERCENT,
    SINGLE_CHAR_TOKEN_EXCLAMATION,
    SINGLE_CHAR_TOKEN_ONE_EQUAL,
    SINGLE_CHAR_TOKEN_LESS_THAN,
    SINGLE_CHAR_TOKEN_GREATER_THAN,
    SINGLE_CHAR_TOKEN_AMPERSAND,
    SINGLE_CHAR_TOKEN_BAR,
    SINGLE_CHAR_TOKEN_CARET,
    SINGLE_CHAR_TOKEN_QUESTION_MARK,
    SINGLE_CHAR_TOKEN_COMMA,
    SINGLE_CHAR_TOKEN_COLON,
    SINGLE_CHAR_END_OF_SINGLE_CHAR_TOKENS
} SingleCharTokenType;

// All the two character tokens the lexer can produce.
typedef enum {
    TWO_CHAR_TOKEN_TWO_AMPERSANDS, // &&
    TWO_CHAR_TOKEN_TWO_BARS, // ||
    TWO_CHAR_TOKEN_LEFT_SHIFT, // <<
    TWO_CHAR_TOKEN_RIGHT_SHIFT, // >>
    TWO_CHAR_END_OF_TWO_CHAR_TOKENS
} TwoCharTokenType;


/**
 * @brief Converts a keyword token type to a token type.
 * 
 * @param type The keyword token type to convert.
 * @return TokenType The corresponding token type, or ERROR if the input is invalid.
 */
TokenType keywordTokenTypeToTokenType(KeywordTokenType type);

/**
 * @brief Converts a single character token type to a token type.
 * 
 * @param type The single character token type to convert.
 * @return TokenType The corresponding token type, or ERROR if the input is invalid.
 */
TokenType singleCharTokenTypeToTokenType(SingleCharTokenType type);

/**
 * @brief Converts a two character token type to a token type.
 * 
 * @param type The two character token type to convert.
 * @return TokenType The corresponding token type, or ERROR if the input is invalid.
 */
TokenType twoCharTokenTypeToTokenType(TwoCharTokenType type);

/**
 * @brief Checks if a token type is a binary operator.
 * 
 * @param type The token type to check.
 * @return int 1 if the token type is a binary operator, 0 otherwise.
 */
int isTokenTypeBinaryOp(TokenType type);

/**
 * @brief Checks if a token type is a unary operator.
 * 
 * @param type The token type to check.
 * @return int 1 if the token type is a unary operator, 0 otherwise.
 */
int isTokenTypeUnaryOp(TokenType type);

/**
 * @brief Converts a binary operator token type to its corresponding operator with equal token type.
 * 
 * @param type The binary operator token type to convert.
 * @return TokenType The corresponding operator with equal token type, or ERROR if the input is invalid.
 */
TokenType binaryOpToOpWithEqual(TokenType type);

/**
 * @brief Converts a subtraction or addition token type to its corresponding double subtraction or addition token type.
 * 
 * @param type The subtraction or addition token type to convert.
 * @return TokenType The corresponding double subtraction or addition token type, or ERROR if the input is invalid.
 */
TokenType subAddtoDoubleSubAdd(TokenType type);

/**
 * @brief Converts a token type to its corresponding token string.
 * 
 * @param type The token type to convert.
 * @return char* The corresponding token string, or NULL if the input is invalid.
 */
char* tokenTypeToToken(TokenType type);

/**
 * @brief Converts a token type to its corresponding unary type.
 * 
 * @param type The token type to convert.
 * @return unaryType The corresponding unary type, or ERROR if the input is invalid.
 */
unaryType tokenTypeToUnaryType(TokenType type);

/**
 * @brief Converts a prefix unary type to its corresponding postfix unary type.
 * 
 * @param type The prefix unary type to convert.
 * @return unaryType The corresponding postfix unary type, or ERROR if the input is invalid.
 */
unaryType prefixToPostfix(unaryType type);