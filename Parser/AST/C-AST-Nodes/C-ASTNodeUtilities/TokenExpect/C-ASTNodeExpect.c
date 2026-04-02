#include "C-ASTNodeExpect.h"
#include <stdlib.h>
#include <stdio.h>
#include "../../../../../Lexer/Tokens/token.h"

int tokensLeft(TokenList* tokens) {
    return tokens->cursor < tokens->currSize;
}

int check(TokenList* tokens, TokenType type) {
    if (!tokensLeft(tokens)) return 0;
    return tokens->tokens[tokens->cursor]->type == type;
}

int checkBinaryOp(TokenList* tokens)  {
    if (!tokensLeft(tokens)) return 0;
    return isBinaryOp(tokens->tokens[tokens->cursor]);
}

int expectConstant(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected constant value but got end of file\n");
        exit(1);
    }

    if (tokens->tokens[tokens->cursor]->type != CONSTANT) {
        fprintf(stderr, "Parser Error: Expected constant value but got %s\n", 
            tokens->tokens[tokens->cursor]->value);
        exit(1);
    }
    return atoi(tokens->tokens[tokens->cursor++]->value);
}

void expect(TokenList* tokens, TokenType type) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected %s but got end of file\n", 
            tokenTypetoToken(type));
        exit(1);
    }

    if (tokens->tokens[tokens->cursor]->type != type) {
        fprintf(stderr, "Parser Error: Expected %s but got %s\n", 
            tokenTypetoToken(type),tokens->tokens[tokens->cursor]->value);
        exit(1);
    }
    tokens->cursor++;
}

char* expectIdentifier(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected identifier but got end of file\n");
        exit(1);
    }
    if (tokens->tokens[tokens->cursor]->type != IDENTIFIER) {
        fprintf(stderr, "Parser Error: Expected identifier but got %s\n", 
            tokens->tokens[tokens->cursor]->value);
        exit(1);
    }
    return tokens->tokens[tokens->cursor++]->value;
}


int isUnaryOp(Token* tok) {
    switch(tok->type) {
        case HYPHEN: break;
        case TWO_HYPHENS:
            fprintf(stderr, "Parser Error: -- is an invalid token\n");
            exit(1);
        case TILDE:
            break;
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

    if (!isUnaryOp(tokens->tokens[tokens->cursor])) {
        fprintf(stderr, "Parser Error: Expected unary operator but got %s\n",
                    tokens->tokens[tokens->cursor]->value);
        exit(1);
    }

    return tokenTypeToUnaryType(tokens->tokens[tokens->cursor++]->type);
}


binType expectBinaryOp(TokenList* tokens) {
    if (!tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Expected binary operator but got end of file\n");
        exit(1);
    }

    if (!isBinaryOp(tokens->tokens[tokens->cursor])) {
        fprintf(stderr, "Parser Error: Expected binary operator but got %s\n",
                    tokens->tokens[tokens->cursor]->value);
        exit(1);
    }

    return tokenTypeToBinType(tokens->tokens[tokens->cursor++]->type);
}

binType tokenTypeToBinType(TokenType type) {
    switch(type) {
        case HYPHEN: return BIN_SUBTRACT;
        case PLUS: return BIN_ADD;
        case ASTERISK: return BIN_MULTIPLY;
        case SLASH: return BIN_DIVIDE;
        case PERCENT: return BIN_MODULO;
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
        default:
            fprintf(stderr, "Parser Error: Expected binary operator but got %s\n",
                    tokenTypetoToken(tok->type));
            exit(1);
    }
}