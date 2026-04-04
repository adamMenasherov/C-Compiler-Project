#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C-ASTNodes.h"
#include "C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodeConstructors.h"

CFactor* C_parseFactor(TokenList* tokens) {
    if (check(tokens, CONSTANT)) {
        return C_CreateFactorFromConstant(C_parseConstant(tokens));
    }

    if (check(tokens, IDENTIFIER)) {
        char* identifier = expectIdentifier(tokens);
        return C_CreateFactorFromVar(C_CreateVar(identifier));
    }

    else if (checkUnaryOp(tokens)) {
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
    while (isBinary && (previous_prec = precedence(TokenList_getAt(tokens, TokenList_getCursor(tokens)))) >= min_prec) {
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

CVar* C_parseVar(TokenList* tokens) {
    char* identifier = expectIdentifier(tokens);
    return C_CreateVar(identifier);
}
CStatement* C_parseStatement(TokenList* tokens) {
    if (check(tokens, RETURN_KEYWORD)) {
        return C_CreateStatement(STMT_RETURN, C_parseReturn(tokens));
    }
    else if (check(tokens, SEMICOLON)) {
        expect(tokens, SEMICOLON);
        return C_CreateStatement(STMT_NULL, NULL);
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
CDecleration* C_parseDecleration(TokenList* tokens) {
    CFactor* fact = NULL;
    expect(tokens, INT_KEYWORD);
    char* identifier = expectIdentifier(tokens);
    if (!identifier){
        fprintf(stderr, "Decleration: Expected identifier and got %s", TokenList_getAt(tokens, TokenList_getCursor(tokens)));
        exit(1);
    }

    if (check(tokens, ONE_EQUAL)) {
        expect(tokens, ONE_EQUAL);
        fact = C_parseFactor(tokens);
        expect(tokens, SEMICOLON);
    }

    expect(tokens, SEMICOLON);
    return C_CreateDecleration(identifier, fact);
}

CBlockItem* C_parseBlockItem(TokenList* tokens);

CReturn* C_parseReturn(TokenList* tokens) {
    expect(tokens, RETURN_KEYWORD);
    CFactor* return_exp = C_parseExpression(tokens, 0);
    if (!return_exp) return NULL;

    expect(tokens, SEMICOLON);
    return C_CreateReturn(return_exp);
}


CFunction* C_parseFunction(TokenList* tokens) {
    CBlockItemList* body = createCBlockItemList();

    expect(tokens, INT_KEYWORD);
    char* identifier = strdup(expectIdentifier(tokens));
    expect(tokens, OPEN_PAREN);
    expect(tokens, VOID_KEYWORD);
    expect(tokens, CLOSE_PAREN);
    expect(tokens, OPEN_BRACE);
    while (!check(tokens, CLOSE_BRACE)) {
        CBlockItem* item = C_parseBlockItem(tokens);
        if (!item) return NULL;
        addCBlockItem(body, item); // Add the block item to the list
    }
    expect(tokens, CLOSE_BRACE);

    return C_CreateFunction(identifier, body);
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
