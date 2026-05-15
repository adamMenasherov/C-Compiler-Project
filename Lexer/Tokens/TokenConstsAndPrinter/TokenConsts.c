#include "TokenConsts.h"

static const TokenType keywordTokenTypeMap[] = {
    [KEYWORD_TOKEN_INT] = INT_KEYWORD,
    [KEYWORD_TOKEN_LONG] = LONG_KEYWORD,
    [KEYWORD_TOKEN_VOID] = VOID_KEYWORD,
    [KEYWORD_TOKEN_STATIC] = STATIC_KEYWORD,
    [KEYWORD_TOKEN_EXTERN] = EXTERN_KEYWORD,
    [KEYWORD_TOKEN_DEFAULT] = DEFAULT_KEYWORD,
    [KEYWORD_TOKEN_RETURN] = RETURN_KEYWORD,
    [KEYWORD_TOKEN_SWITCH] = SWITCH_KEYWORD,    
    [KEYWORD_TOKEN_IF] = IF_KEYWORD,
    [KEYWORD_TOKEN_CASE] = CASE_KEYWORD,
    [KEYWORD_TOKEN_ELSE] = ELSE_KEYWORD,
    [KEYWORD_TOKEN_WHILE] = WHILE_KEYWORD,
    [KEYWORD_TOKEN_FOR] = FOR_KEYWORD,
    [KEYWORD_TOKEN_DO] = DO_KEYWORD,
    [KEYWORD_TOKEN_BREAK] = BREAK_KEYWORD,
    [KEYWORD_TOKEN_CONTINUE] = CONTINUE_KEYWORD
};

static const TokenType singleCharTokenTypeMap[] = {
    [SINGLE_CHAR_TOKEN_OPEN_PAREN] = OPEN_PAREN,
    [SINGLE_CHAR_TOKEN_CLOSE_PAREN] = CLOSE_PAREN,
    [SINGLE_CHAR_TOKEN_OPEN_BRACE] = OPEN_BRACE,
    [SINGLE_CHAR_TOKEN_CLOSE_BRACE] = CLOSE_BRACE,
    [SINGLE_CHAR_TOKEN_SEMICOLON] = SEMICOLON,
    [SINGLE_CHAR_TOKEN_TILDE] = TILDE,
    [SINGLE_CHAR_TOKEN_HYPHEN] = HYPHEN,
    [SINGLE_CHAR_TOKEN_PLUS] = PLUS,
    [SINGLE_CHAR_TOKEN_ASTERISK] = ASTERISK,
    [SINGLE_CHAR_TOKEN_SLASH] = SLASH,
    [SINGLE_CHAR_TOKEN_PERCENT] = PERCENT,
    [SINGLE_CHAR_TOKEN_EXCLAMATION] = EXCLAMATION,
    [SINGLE_CHAR_TOKEN_ONE_EQUAL] = ONE_EQUAL,
    [SINGLE_CHAR_TOKEN_LESS_THAN] = LESS_THAN,
    [SINGLE_CHAR_TOKEN_GREATER_THAN] = GREATER_THAN,
    [SINGLE_CHAR_TOKEN_AMPERSAND] = AMPERSAND,
    [SINGLE_CHAR_TOKEN_BAR] = BAR,
    [SINGLE_CHAR_TOKEN_CARET] = CARET,
    [SINGLE_CHAR_TOKEN_QUESTION_MARK] = QUESTION_MARK,
    [SINGLE_CHAR_TOKEN_COMMA] = COMMA,
    [SINGLE_CHAR_TOKEN_COLON] = COLON
};

static const TokenType twoCharTokenTypeMap[] = {
    [TWO_CHAR_TOKEN_TWO_AMPERSANDS] = TWO_AMPERSANDS,
    [TWO_CHAR_TOKEN_TWO_BARS] = TWO_BARS,
    [TWO_CHAR_TOKEN_LEFT_SHIFT] = LEFT_SHIFT,
    [TWO_CHAR_TOKEN_RIGHT_SHIFT] = RIGHT_SHIFT
};

TokenType keywordTokenTypeToTokenType(KeywordTokenType type) {
    if (type < 0 || type >= KEYWORD_TOKEN_END_OF_KEYWORDS) return ERROR;
    return keywordTokenTypeMap[type];
}

TokenType singleCharTokenTypeToTokenType(SingleCharTokenType type) {
    if (type < 0 || type >= SINGLE_CHAR_END_OF_SINGLE_CHAR_TOKENS) return ERROR;
    return singleCharTokenTypeMap[type];
}

TokenType twoCharTokenTypeToTokenType(TwoCharTokenType type) {
    if (type < 0 || type >= TWO_CHAR_END_OF_TWO_CHAR_TOKENS) return ERROR;
    return twoCharTokenTypeMap[type];
}

const char *tokenTypeStr[] = {
    [IDENTIFIER] = "IDENTIFIER",
    [CONSTANT] = "CONSTANT",
    [LONG_CONSTANT] = "LONG_CONSTANT",
    [LONG_KEYWORD] = "LONG_KEYWORD",
    [INT_KEYWORD] = "INT_KEYWORD",
    [SWITCH_KEYWORD] = "SWITCH_KEYWORD",
    [VOID_KEYWORD] = "VOID_KEYWORD",
    [STATIC_KEYWORD] = "STATIC_KEYWORD",
    [DEFAULT_KEYWORD] = "DEFAULT_KEYWORD",
    [CASE_KEYWORD] = "CASE_KEYWORD",
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
    [LONG_CONSTANT] = "",
    [LONG_KEYWORD] = "long",
    [INT_KEYWORD] = "int",
    [VOID_KEYWORD] = "void",
    [CASE_KEYWORD] = "case",
    [STATIC_KEYWORD] = "static",
    [DEFAULT_KEYWORD] = "default",
    [EXTERN_KEYWORD] = "extern",
    [RETURN_KEYWORD] = "return",
    [SWITCH_KEYWORD] = "switch",
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


int isBinaryOp(TokenType type) {
    switch(type) {
        case PLUS:
        case HYPHEN:
        case ASTERISK:
        case SLASH:
        case PERCENT:
        case EXCLAMATION:
        case TWO_AMPERSANDS:
        case TWO_BARS:
        case ONE_EQUAL:
        case TWO_EQUALS:
        case NOT_EQUAL:
        case LESS_THAN:
        case GREATER_THAN:
        case LESS_EQUAL:
        case GREATER_EQUAL:
        case PLUS_EQUAL:
        case MINUS_EQUAL:
        case STAR_EQUAL:
        case SLASH_EQUAL:
        case PERCENT_EQUAL:
        case AMPERSAND:
        case BAR:
        case CARET:
        case LEFT_SHIFT:
        case RIGHT_SHIFT:
            return 1;
        default: return 0;
    }
}

int isUnaryOp(TokenType type) {
    switch(type) {
        case PLUS:
        case HYPHEN:
        case EXCLAMATION:
        case TILDE:
            return 1;
        default: return 0;
    }
}

TokenType binaryOpToOpWithEqual(TokenType type) {
    switch(type) {
        case PLUS: return PLUS_EQUAL;
        case HYPHEN: return MINUS_EQUAL;
        case ASTERISK: return STAR_EQUAL;
        case SLASH: return SLASH_EQUAL;
        case PERCENT: return PERCENT_EQUAL;
        case AMPERSAND: return AMPERSAND_EQUAL;
        case BAR: return BAR_EQUAL;
        case CARET: return CARET_EQUAL;
        case LEFT_SHIFT: return LEFT_SHIFT_EQUAL;
        case RIGHT_SHIFT: return RIGHT_SHIFT_EQUAL;
        case ONE_EQUAL: return TWO_EQUALS;
        case LESS_THAN: return LESS_EQUAL;
        case GREATER_THAN: return GREATER_EQUAL;
        case EXCLAMATION: return NOT_EQUAL;
        default: return ERROR;
    }
}

TokenType subAddtoDoubleSubAdd(TokenType type) {
    switch(type) {
        case PLUS: return TWO_PLUS;
        case HYPHEN: return TWO_HYPHENS;
        default: return ERROR;
    }
}