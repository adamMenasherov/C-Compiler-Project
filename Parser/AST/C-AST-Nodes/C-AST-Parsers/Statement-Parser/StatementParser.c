#include "StatementParser.h"
#include <stdio.h>
#include <stdlib.h>
#include "../C-ParsersInclude.h"

CStatement* C_parseStatement(TokenList* tokens) {
    if (check(tokens, RETURN_KEYWORD)) {
        return C_CreateStatement(STMT_RETURN, C_parseReturn(tokens));
    }
    if (check(tokens, IF_KEYWORD)) {
        return C_CreateStatement(STMT_IF, C_parseIf(tokens));
    }
    if (check(tokens, SWITCH_KEYWORD)) {
        return C_CreateStatement(STMT_SWITCH, C_parseSwitch(tokens));
    }

    else if (check(tokens, SEMICOLON)) {
        expect(tokens, SEMICOLON);
        return C_CreateStatement(STMT_NULL, NULL);
    }
    else if (checkIsLoopStatement(tokens)) {
        return C_parseLoopStatement(tokens);
    }

    else if (check(tokens, OPEN_BRACE)) {
        expect(tokens, OPEN_BRACE);
        CBlock* blockItemType = C_parseBlock(tokens);
        expect(tokens, CLOSE_BRACE);
        return C_CreateStatement(STMT_COMPOUND, C_CreateCompound(blockItemType));
    }

    else if (checkFactorStart(tokens)) {
        CFactor* exp = C_parseExpression(tokens, 0);
        expect(tokens, SEMICOLON);
        return C_CreateStatement(STMT_EXPRESSION, exp);
    }
    else {
        fprintf(stderr, "Parser Error: Invalid statement\n");
        exit(1);
    }        
}

CReturn* C_parseReturn(TokenList* tokens) {
    expect(tokens, RETURN_KEYWORD);
    CFactor* return_exp = C_parseExpression(tokens, 0);
    if (!return_exp) return NULL;

    expect(tokens, SEMICOLON);
    return C_CreateReturn(return_exp);
}
