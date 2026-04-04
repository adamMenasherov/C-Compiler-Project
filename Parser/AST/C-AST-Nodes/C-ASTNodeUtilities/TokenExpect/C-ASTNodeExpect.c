#include "C-ASTNodeExpect.h"
#include <stdlib.h>
#include <stdio.h>
#include "../../../../../Lexer/Tokens/token.h"

int tokensLeft(TokenList* tokens) {
    return TokenList_getCursor(tokens) < TokenList_size(tokens);
}

int check(TokenList* tokens, TokenType type) {
    if (!tokensLeft(tokens)) return 0;
    Token* tok = TokenList_getAt(tokens, TokenList_getCursor(tokens));
    return tok ? tok->type == type : 0;
}

int checkBinaryOp(TokenList* tokens)  {
    if (!tokensLeft(tokens)) return 0;
    Token* tok = TokenList_getAt(tokens, TokenList_getCursor(tokens));
    return tok ? isBinaryOp(tok) : 0;
}

int checkUnaryOp(TokenList* tokens) {
    if (!tokensLeft(tokens)) return 0;
    Token* tok = TokenList_getAt(tokens, TokenList_getCursor(tokens));
    return tok ? isUnaryOp(tok) : 0;
}

int expectConstant(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected constant value but got end of file\n");
        exit(1);
    }

    int cursor = TokenList_getCursor(tokens);
    Token* tok = TokenList_getAt(tokens, cursor);
    if (!tok || tok->type != CONSTANT) {
        fprintf(stderr, "Parser Error: Expected constant value but got %s\n", 
            tok ? tok->value : "NULL");
        exit(1);
    }
    TokenList_setCursor(tokens, cursor + 1);
    return atoi(tok->value);
}

void expect(TokenList* tokens, TokenType type) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected %s but got end of file\n", 
            tokenTypetoToken(type));
        exit(1);
    }

    int cursor = TokenList_getCursor(tokens);
    Token* tok = TokenList_getAt(tokens, cursor);
    if (!tok || tok->type != type) {
        fprintf(stderr, "Parser Error: Expected %s but got %s\n", 
            tokenTypetoToken(type), tok ? tok->value : "NULL");
        exit(1);
    }
    TokenList_setCursor(tokens, cursor + 1);
}

char* expectIdentifier(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected identifier but got end of file\n");
        exit(1);
    }
    int cursor = TokenList_getCursor(tokens);
    Token* tok = TokenList_getAt(tokens, cursor);
    if (!tok || tok->type != IDENTIFIER) {
        fprintf(stderr, "Parser Error: Expected identifier but got %s\n", 
            tok ? tok->value : "NULL");
        exit(1);
    }
    TokenList_setCursor(tokens, cursor + 1);
    return tok->value;
}


int isUnaryOp(Token* tok) {
    switch(tok->type) {
        case TILDE:
        case HYPHEN:
        case EXCLAMATION: 
            break;
        case TWO_HYPHENS:
            fprintf(stderr, "Parser Error: -- is an invalid token\n");
            exit(1);
        default: return 0;
    }

    return 1;
}


int isBinaryOp(Token* tok) {
    switch(tok->type) {
        case TWO_HYPHENS:
            fprintf(stderr, "Parser Error: %s is an invalid token\n", tok->value);
            exit(1);
        case HYPHEN:
        case PLUS:
        case ASTERISK:
        case SLASH:
        case PERCENT:
        case TWO_AMPERSANDS:
        case TWO_BARS:
        case TWO_EQUALS:
        case NOT_EQUAL:
        case LESS_THAN:
        case GREATER_THAN:
        case LESS_EQUAL:
        case GREATER_EQUAL:
        case ONE_EQUAL:
            break;
        default: return 0;
    }

    return 1;
}

unaryType expectUnaryOp(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected unary operator but got end of file\n");
        exit(1);
    }

    int cursor = TokenList_getCursor(tokens);
    Token* tok = TokenList_getAt(tokens, cursor);
    if (!tok || !isUnaryOp(tok)) {
        fprintf(stderr, "Parser Error: Expected unary operator but got %s\n",
                    tok ? tok->value : "NULL");
        exit(1);
    }

    TokenList_setCursor(tokens, cursor + 1);
    return tokenTypeToUnaryType(tok->type);
}


binType expectBinaryOp(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected binary operator but got end of file\n");
        exit(1);
    }

    int cursor = TokenList_getCursor(tokens);
    Token* tok = TokenList_getAt(tokens, cursor);
    if (!tok || !isBinaryOp(tok)) {
        fprintf(stderr, "Parser Error: Expected binary operator but got %s\n",
                    tok ? tok->value : "NULL");
        exit(1);
    }

    TokenList_setCursor(tokens, cursor + 1);
    return tokenTypeToBinType(tok->type);
}

binType tokenTypeToBinType(TokenType type) {
    switch(type) {
        case HYPHEN: return BIN_SUBTRACT;
        case PLUS: return BIN_ADD;
        case ASTERISK: return BIN_MULTIPLY;
        case SLASH: return BIN_DIVIDE;
        case PERCENT: return BIN_REMAINDER;
        case TWO_AMPERSANDS: return BIN_AND;
        case TWO_BARS: return BIN_OR;
        case TWO_EQUALS: return BIN_EQUALS;
        case NOT_EQUAL: return BIN_NOT_EQUALS;
        case LESS_THAN: return BIN_LESS_THAN;
        case GREATER_THAN: return BIN_GREATER_THAN;
        case LESS_EQUAL: return BIN_LESS_EQUAL;
        case GREATER_EQUAL: return BIN_GREATER_EQUAL;
        default:
            fprintf(stderr, "Parser Error: Expected binary operator but got %s\n",
                    tokenTypetoToken(type));
            exit(1);
    }    
}

int precedence(Token* tok) {
    switch (tok->type) {
        case ASTERISK:
        case SLASH:
        case PERCENT:
            return 50;
        case HYPHEN:
        case PLUS:
            return 45;
        case LESS_THAN:
        case GREATER_THAN:
        case LESS_EQUAL:
        case GREATER_EQUAL:
            return 35;
        case TWO_EQUALS:
        case NOT_EQUAL:
            return 30;
        case TWO_AMPERSANDS:
            return 10;
        case TWO_BARS:
            return 5;
        case ONE_EQUAL:
            return 1;
        default:
            fprintf(stderr, "Parser Error: Expected binary operator but got %s\n",
                    tokenTypetoToken(tok->type));
            exit(1);
    }
}


int checkFactorStart(TokenList* tokens) {
    if (!tokensLeft(tokens)) return 0;
    return check(tokens, CONSTANT) || check(tokens, IDENTIFIER) || checkUnaryOp(tokens) 
    || checkBinaryOp(tokens) 
    || check(tokens, OPEN_PAREN);
}