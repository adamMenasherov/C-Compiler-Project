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

int checkCompoundAssignment(TokenList* tokens) {
    if (!tokensLeft(tokens)) return 0;
    Token* tok = TokenList_getAt(tokens, TokenList_getCursor(tokens));
    return tok ? (tok->type == PLUS_EQUAL || tok->type == MINUS_EQUAL || 
                   tok->type == STAR_EQUAL || tok->type == SLASH_EQUAL || 
                   tok->type == PERCENT_EQUAL) : 0;
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

binType compoundAssignmentToBinType(TokenType type) {
    switch(type) {
        case PLUS_EQUAL: return BIN_ADD;
        case MINUS_EQUAL: return BIN_SUBTRACT;
        case STAR_EQUAL: return BIN_MULTIPLY;
        case SLASH_EQUAL: return BIN_DIVIDE;
        case PERCENT_EQUAL: return BIN_REMAINDER;
        default:
            fprintf(stderr, "Parser Error: Expected compound assignment operator but got %s\n",
                    tokenTypeToToken(type));
            exit(1);
    }
}

void expect(TokenList* tokens, TokenType type) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected %s but got end of file\n", 
            tokenTypeToToken(type));
        exit(1);
    }

    int cursor = TokenList_getCursor(tokens);
    Token* tok = TokenList_getAt(tokens, cursor);
    if (!tok || tok->type != type) {
        fprintf(stderr, "Parser Error: Expected %s but got %s\n", 
            tokenTypeToToken(type), tok ? tok->value : "NULL");
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

void expectCompoundAssignment(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected compound assignment operator but got end of file\n");
        exit(1);
    }
    int cursor = TokenList_getCursor(tokens);
    Token* tok = TokenList_getAt(tokens, cursor);
    if (!tok || !(tok->type == PLUS_EQUAL || tok->type == MINUS_EQUAL || 
                   tok->type == STAR_EQUAL || tok->type == SLASH_EQUAL || 
                   tok->type == PERCENT_EQUAL)) {
        fprintf(stderr, "Parser Error: Expected compound assignment operator but got %s\n", 
            tok ? tok->value : "NULL");
        exit(1);
    }
    TokenList_setCursor(tokens, cursor + 1);
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
        case AMPERSAND: return BIN_BITWISE_AND;
        case BAR: return BIN_BITWISE_OR;
        case CARET: return BIN_BITWISE_XOR;
        case LEFT_SHIFT: return BIN_LEFT_SHIFT;
        case RIGHT_SHIFT: return BIN_RIGHT_SHIFT;
        default:
            fprintf(stderr, "Parser Error: Expected binary operator but got %s\n",
                    tokenTypeToToken(type));
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
        case LEFT_SHIFT:
        case RIGHT_SHIFT:
            return 40;
        case LESS_THAN:
        case GREATER_THAN:
        case LESS_EQUAL:
        case GREATER_EQUAL:
            return 35;
        case TWO_EQUALS:
        case NOT_EQUAL:
            return 30;
        case AMPERSAND:
            return 25;
        case CARET:
            return 20;
        case BAR:
            return 15;
        case TWO_AMPERSANDS:
            return 10;
        case TWO_BARS:
            return 5;
        case ONE_EQUAL:
        case PLUS_EQUAL:
        case MINUS_EQUAL:
        case STAR_EQUAL:
        case SLASH_EQUAL:
        case PERCENT_EQUAL:
        case AMPERSAND_EQUAL:
        case BAR_EQUAL:
        case CARET_EQUAL:
        case LEFT_SHIFT_EQUAL:
        case RIGHT_SHIFT_EQUAL:
            return 1;
        default:
            fprintf(stderr, "Parser Error: Expected binary operator but got %s\n",
                    tokenTypeToToken(tok->type));
            exit(1);
    }
}


int checkFactorStart(TokenList* tokens) {
    if (!tokensLeft(tokens)) return 0;
    return check(tokens, CONSTANT) || check(tokens, IDENTIFIER) || checkUnaryOp(tokens) 
    || checkBinaryOp(tokens) 
    || check(tokens, OPEN_PAREN);
}