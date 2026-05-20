#include "IfParser.h"
#include "../C-ParsersInclude.h"
#include <stdio.h>

static void addCaseToSwitch(CSwitch* switchBlock, CConstant* caseConst, CStatement* caseStmt, int hasBreak) {
    CCase* switchCase = C_CreateCase(caseConst, caseStmt, hasBreak);
    if (!switchCase) return;
    switchBlock->cases[switchBlock->caseCount++] = switchCase;
}

static void handleDefaultCase(CSwitch* switchBlock, TokenList* tokens) {
    if (switchBlock->defaultCase) {
        fprintf(stderr, "Parser Error: Multiple default cases in switch\n");
        exit(1);
    }
    expect(tokens, DEFAULT_KEYWORD);
    expect(tokens, COLON);
    switchBlock->defaultCase = C_parseStatement(tokens);

    if (check(tokens, BREAK_KEYWORD)) {
        expect(tokens, BREAK_KEYWORD);
        expect(tokens, SEMICOLON);
    }
}


CIf* C_parseIf(TokenList* tokens) {
    expect(tokens, IF_KEYWORD);
    expect(tokens, OPEN_PAREN);
    CFactor* condition = C_parseExpression(tokens, 0);
    expect(tokens, CLOSE_PAREN);
    CStatement* then_stmt = C_parseStatement(tokens);
    CStatement* else_stmt = NULL;
    if (check(tokens, ELSE_KEYWORD)) {
        expect(tokens, ELSE_KEYWORD);
        else_stmt = C_parseStatement(tokens);
        return C_CreateIf(IF_WITH_ELSE, condition, then_stmt, else_stmt);
    }
    return C_CreateIf(IF_WITHOUT_ELSE, condition, then_stmt, else_stmt);
}


CSwitch* C_parseSwitch(TokenList* tokens) {
    CSwitch* switchBlock;
    expect(tokens, SWITCH_KEYWORD);
    expect(tokens, OPEN_PAREN);
    CFactor* exp = C_parseExpression(tokens, 0);
    expect(tokens, CLOSE_PAREN);
    expect(tokens, OPEN_BRACE);
    switchBlock  = C_CreateSwitch(exp);
    if (!switchBlock) return NULL;

    while (!check(tokens, CLOSE_BRACE)) {
        int hasBreak = 0;
        if (check(tokens, DEFAULT_KEYWORD))
            handleDefaultCase(switchBlock, tokens);
        else {
            expect(tokens, CASE_KEYWORD);
            CConstant* caseConst = C_parseConstant(tokens); 
            expect(tokens, COLON);
            CStatement* caseStmt = C_parseStatement(tokens);
            
            if (check(tokens, BREAK_KEYWORD)) {
                expect(tokens, BREAK_KEYWORD);
                expect(tokens, SEMICOLON);
                hasBreak = 1;
            }
                
            addCaseToSwitch(switchBlock, caseConst, caseStmt, hasBreak);
        }   
    }

    expect(tokens, CLOSE_BRACE);
    return switchBlock;
}