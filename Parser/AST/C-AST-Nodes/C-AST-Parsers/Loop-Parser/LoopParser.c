#include "LoopParser.h"
#include <stdio.h>
#include <stdlib.h>
#include "../C-ParsersInclude.h"

typedef CStatement* (*LoopStatementHandler)(TokenList* tokens);

static CStatement* handleBreak(TokenList* tokens) {
    expect(tokens, BREAK_KEYWORD);
    expect(tokens, SEMICOLON);
    return C_CreateStatement(STMT_BREAK, C_CreateLoopStmt());
}

static CStatement* handleContinue(TokenList* tokens) {
    expect(tokens, CONTINUE_KEYWORD);
    expect(tokens, SEMICOLON);
    return C_CreateStatement(STMT_CONTINUE, C_CreateLoopStmt());
}

static CStatement* handleWhile(TokenList* tokens) {
    return C_CreateStatement(STMT_WHILE, C_parseWhile(tokens));
}

static CStatement* handleDoWhile(TokenList* tokens) {
    return C_CreateStatement(STMT_DO_WHILE, C_parseDoWhile(tokens));
}

static CStatement* handleFor(TokenList* tokens) {
    return C_CreateStatement(STMT_FOR, C_parseFor(tokens));
}

static const LoopStatementHandler loopStatementHandlers[] = {
    [STMT_BREAK] = handleBreak,
    [STMT_CONTINUE] = handleContinue,
    [STMT_WHILE] = handleWhile,
    [STMT_DO_WHILE] = handleDoWhile,
    [STMT_FOR] = handleFor
};


CStatement* C_parseLoopStatement(TokenList* tokens) {
    statementType type = loopStatementKeywordToStatementType(tokens);
    if (type < 0 || type >= sizeof(loopStatementHandlers) / sizeof(loopStatementHandlers[0]) || !loopStatementHandlers[type]) {
        fprintf(stderr, "Invalid loop statement type in C_parseLoopStatement\n");
        exit(1);
    }
    return loopStatementHandlers[type](tokens);
}

CForInit* C_parseForInit(TokenList* tokens) {
    if (checkSpecifier(tokens)) {
        CType* varType;
        StorageClass storageClass;
        C_parseTypeAndStorageClass(tokens, &varType, &storageClass);

        char* identifier = expectIdentifier(tokens);
        if (!identifier){
            fprintf(stderr, "Decleration: Expected identifier and got %s", TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array))->value);
            exit(1);
        }

        CDeclaration* decl = C_parseVarDeclaration(tokens, identifier, varType, storageClass);
        return C_CreateForInit(FOR_INIT_DECL, decl);
    }
    else if (!check(tokens, SEMICOLON)) {
        CFactor* exp = C_parseExpression(tokens, 0);
        expect(tokens, SEMICOLON);
        return C_CreateForInit(FOR_INIT_EXP, exp);
    }
    else {
        expect(tokens, SEMICOLON);
        return C_CreateForInit(FOR_INIT_WITHOUT, NULL);
    }
}

CLoop* C_parseWhile(TokenList* tokens) {
    expect(tokens, WHILE_KEYWORD);
    expect(tokens, OPEN_PAREN);
    CFactor* condition = C_parseExpression(tokens, 0);
    expect(tokens, CLOSE_PAREN);
    CStatement* body = C_parseStatement(tokens);
    return C_CreateLoop(condition, body);
}

CLoop* C_parseDoWhile(TokenList* tokens) {
    expect(tokens, DO_KEYWORD);
    CStatement* body = C_parseStatement(tokens);
    expect(tokens, WHILE_KEYWORD);
    expect(tokens, OPEN_PAREN);
    CFactor* condition = C_parseExpression(tokens, 0);
    expect(tokens, CLOSE_PAREN);
    expect(tokens, SEMICOLON);
    return C_CreateLoop(condition, body);
}

CForLoop* C_parseFor(TokenList* tokens) {
    expect(tokens, FOR_KEYWORD);
    expect(tokens, OPEN_PAREN);
    CForInit* init = C_parseForInit(tokens);
    CFactor* condition = NULL;
    if (!check(tokens, SEMICOLON)) {
        condition = C_parseExpression(tokens, 0);
    }
    expect(tokens, SEMICOLON);
    CFactor* post = NULL;
    if (!check(tokens, CLOSE_PAREN)) {
        post = C_parseExpression(tokens, 0);
    }
    expect(tokens, CLOSE_PAREN);
    CStatement* body = C_parseStatement(tokens);
    return C_CreateForLoop(init, condition, post, body);
}