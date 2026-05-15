#include "StatementResolver.h"
#include "../ExpressionResolver/ExpressionResolver.h"
#include "../BlockResolver/BlockResolver.h"
#include "../VariableResolver/VariableResolver.h"
#include "../../TypeChecking/DeclarationTypeChecker/DeclarationTypeChecker.h"
#include "../../../utils/ScopeUtils/ScopeUtils.h"
#include <stdio.h>
#include <stdlib.h>


static void handleForInitDecl(CForInit* init, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (init->decl->decl.variableDecl.storageClass != SPEC_NULL) {
        fprintf(stderr, "Semantic Error: A for-loop variable declaration cannot have a storage class\n");
        exit(1);
    }
    resolveLocalVarDeclaration(init->decl, varMap, symbolTable);
    typeCheckLocalVariableDeclaration(init->decl, symbolTable);
}

static void handleForInitExp(CForInit* init, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveExpression(init->exp, varMap, symbolTable);
}

static void handleForInitWithout(CForInit* init, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {}

typedef void (*ForInitHandler)(CForInit*, SemanticIdentifierMap*, SymbolTable*);

static const ForInitHandler forInitHandlers[] = {
    [FOR_INIT_DECL]    = handleForInitDecl,
    [FOR_INIT_EXP]     = handleForInitExp,
    [FOR_INIT_WITHOUT] = handleForInitWithout,
};

static void resolveForInit(CForInit* init, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!init) return;
    ForInitHandler handler = forInitHandlers[init->type];
    if (!handler) return;
    handler(init, varMap, symbolTable);
}


static void handleStmtExpression(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveExpression(stmt->stmt.exp, varMap, symbolTable);
}

static void handleStmtReturn(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveExpression(stmt->stmt.ret->exp, varMap, symbolTable);
}

static void handleStmtIf(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveExpression(stmt->stmt.if_stmt->condition, varMap, symbolTable);
    resolveStatement(stmt->stmt.if_stmt->then, varMap, symbolTable);
    if (stmt->stmt.if_stmt->else_stmt) {
        resolveStatement(stmt->stmt.if_stmt->else_stmt, varMap, symbolTable);
    }
}

static void handleStmtCompound(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    SemanticIdentifierMap* newVarMap = copySemanticIdentifierMap(varMap);
    markAllEntriesOutOfCurrentScope(newVarMap);
    resolveBlock(stmt->stmt.compound_stmt->block, newVarMap, symbolTable);
    freeSemanticIdentifierMap(newVarMap);
}

static void handleStmtFor(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    SemanticIdentifierMap* newVarMap = copySemanticIdentifierMap(varMap);
    markAllEntriesOutOfCurrentScope(newVarMap);
    resolveForInit(stmt->stmt.for_stmt->init, newVarMap, symbolTable);
    resolveExpression(stmt->stmt.for_stmt->condition, newVarMap, symbolTable);
    resolveExpression(stmt->stmt.for_stmt->post, newVarMap, symbolTable);
    resolveStatement(stmt->stmt.for_stmt->body, newVarMap, symbolTable);
    freeSemanticIdentifierMap(newVarMap);
}

static void handleStmtWhile(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    SemanticIdentifierMap* newVarMap = copySemanticIdentifierMap(varMap);
    markAllEntriesOutOfCurrentScope(newVarMap);
    resolveExpression(stmt->stmt.while_stmt->condition, newVarMap, symbolTable);
    resolveStatement(stmt->stmt.while_stmt->body, newVarMap, symbolTable);
    freeSemanticIdentifierMap(newVarMap);
}

static void handleStmtDoWhile(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    SemanticIdentifierMap* newVarMap = copySemanticIdentifierMap(varMap);
    markAllEntriesOutOfCurrentScope(newVarMap);
    resolveStatement(stmt->stmt.do_while_stmt->body, newVarMap, symbolTable);
    resolveExpression(stmt->stmt.do_while_stmt->condition, newVarMap, symbolTable);
    freeSemanticIdentifierMap(newVarMap);
}

static void handleStmtSwitch(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveExpression(stmt->stmt.switch_stmt->switchExp, varMap, symbolTable);
    for (int i = 0; i < stmt->stmt.switch_stmt->caseCount; i++) {
        resolveStatement(stmt->stmt.switch_stmt->cases[i]->body, varMap, symbolTable);
    }
    if (stmt->stmt.switch_stmt->defaultCase) {
        resolveStatement(stmt->stmt.switch_stmt->defaultCase, varMap, symbolTable);
    }
}

static void handleStmtNoop(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {}

typedef void (*StmtHandler)(CStatement*, SemanticIdentifierMap*, SymbolTable*);
static const StmtHandler stmtHandlers[] = {
    [STMT_EXPRESSION] = handleStmtExpression,
    [STMT_RETURN]     = handleStmtReturn,
    [STMT_IF]         = handleStmtIf,
    [STMT_COMPOUND]   = handleStmtCompound,
    [STMT_FOR]        = handleStmtFor,
    [STMT_WHILE]      = handleStmtWhile,
    [STMT_DO_WHILE]   = handleStmtDoWhile,
    [STMT_SWITCH] = handleStmtSwitch,
    [STMT_NULL]       = handleStmtNoop,
};

void resolveStatement(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!stmt) return;
    StmtHandler handler = stmtHandlers[stmt->type];
    if (!handler) return;
    handler(stmt, varMap, symbolTable);
}
