#include "C-ASTNodes.h"
#include "C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodeConstructors.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


CFactor* C_parseFactor(TokenList* tokens) {
    if (check(tokens, CONSTANT)) {
        return C_CreateFactorFromConstant(C_parseConstant(tokens));
    }
    else if (isUnaryOp(tokens->tokens[tokens->cursor])) {
        unaryType type = expectUnaryOp(tokens);
        CFactor* inner_exp = C_parseFactor(tokens);
        return C_CreateFactorFromUnary(C_CreateUnary(type, inner_exp));
    }

    else if (check(tokens, OPEN_PAREN)) {
        expect(tokens, OPEN_PAREN);
        CFactor* inner_exp = C_parseExpression(tokens, 0);
        expect(tokens, CLOSE_PAREN);
        return inner_exp;
    }
    else {
        fprintf(stderr, "Expression is invalid\n");
        exit(1);
    } 

    return NULL;
}


CFactor* C_parseExpression(TokenList* tokens, int min_prec) {
    CFactor* left = C_parseFactor(tokens);
    if (!left) return NULL;
    int isBinary = checkBinaryOp(tokens), previous_prec; 
    while (isBinary && (previous_prec = precedence(tokens->tokens[tokens->cursor])) >= min_prec) {
        binType type = expectBinaryOp(tokens);
        CFactor* right = C_parseExpression(tokens, previous_prec + 1);
        if (!right) return NULL;
        CBinary* new_left = C_CreateBinary(type, left, right);
        left = C_CreateFactorFromBinary(new_left);
        isBinary = checkBinaryOp(tokens);
    }
    return left;
    
}

CConstant* C_parseConstant(TokenList* tokens) {
    int val = expectConstant(tokens);
    return C_CreateConstant(val);
}


CReturn* C_parseReturn(TokenList* tokens) {
    expect(tokens, RETURN_KEYWORD);
    CFactor* return_exp = C_parseExpression(tokens, 0);
    if (!return_exp) return NULL;

    expect(tokens, SEMICOLON);
    return C_CreateReturn(return_exp);
}


CFunction* C_parseFunction(TokenList* tokens) {
    expect(tokens, INT_KEYWORD);
    char* identifier = strdup(expectIdentifier(tokens));
    expect(tokens, OPEN_PAREN);
    expect(tokens, VOID_KEYWORD);
    expect(tokens, CLOSE_PAREN);
    expect(tokens, OPEN_BRACE);
    CReturn* statement = C_parseReturn(tokens);
    if (!statement) return NULL;

    expect(tokens, CLOSE_BRACE);

    return C_CreateFunction(identifier, statement);
}


CProgram* C_parseProgram(TokenList* tokens) {
    CFunction* func = C_parseFunction(tokens);
    if (!func) return NULL;

    if (tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Invalid tokens left\n");
        exit(1);
    }

    return C_CreateProgram(func);
}
