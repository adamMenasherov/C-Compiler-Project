#include "C-ASTNodes.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


CExpression* C_parseExpression(TokenList* tokens) {
    if (check(tokens, CONSTANT)) {
        return C_CreateExpressionFromConstant(C_parseConstant(tokens));
    }
    else if (isUnaryOp(tokens->tokens[tokens->cursor])) {
        unaryType type = expectUnaryOp(tokens);
        CExpression* inner_exp = C_parseExpression(tokens);
        return C_CreateExpressionFromUnary(C_CreateUnary(type, inner_exp));
    }

    else if (check(tokens, OPEN_PAREN)) {
        expect(tokens, OPEN_PAREN);
        CExpression* inner_exp = C_parseExpression(tokens);
        expect(tokens, CLOSE_PAREN);
        return inner_exp;
    }
    else {
        fprintf(stderr, "Expression is invalid\n");
        exit(1);
    } 

    return NULL;
}


CConstant* C_parseConstant(TokenList* tokens) {
    int val = expectConstant(tokens);
    return C_CreateConstant(val);
}


CReturn* C_parseReturn(TokenList* tokens) {
    expect(tokens, RETURN_KEYWORD);
    CExpression* return_exp = C_parseExpression(tokens);
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
