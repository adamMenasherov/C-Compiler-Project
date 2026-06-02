#include "StatementTypeChecker.h"
#include "../ExpressionTypeChecker/ExpressionTypeChecker.h"
#include "../TypeChecker/TypeChecker.h"
#include "../DeclarationTypeChecker/DeclarationTypeChecker.h"
#include "../../../../Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodeConstructors.h"
#include "../../../utils/SemanticUtils/SemanticUtils.h"
#include <stdio.h>
#include <stdlib.h>

static CType* currentFuncReturnType = NULL;

void setCurrentFunctionReturnType(CType* retType) {
    currentFuncReturnType = retType;
}

static CFactor* convertReturnExprToFuncType(CFactor* expr, CType* targetType) {
    if (!expr || ctypeEqual(expr->valueType, targetType)) return expr;
    CCast* castNode = C_CreateCast(targetType, expr);
    CFactor* castExpr = C_CreateFactorFromCast(castNode);
    if (!castExpr) {
        fprintf(stderr, "Semantic Error: Failed to create implicit cast for return expression\n");
        exit(1);
    }
    setType(castExpr, targetType);
    return castExpr;
}

static void handleTypeCheckReturn(CStatement* stmt, SymbolTable* symbolTable) {
    typeCheckExpression(stmt->stmt.ret->exp, symbolTable);
    stmt->stmt.ret->exp = convertReturnExprToFuncType(stmt->stmt.ret->exp, currentFuncReturnType);
}

static void handleTypeCheckExpression(CStatement* stmt, SymbolTable* symbolTable) {
    typeCheckExpression(stmt->stmt.exp, symbolTable);
}

static void handleTypeCheckIf(CStatement* stmt, SymbolTable* symbolTable) {
    typeCheckExpression(stmt->stmt.if_stmt->condition, symbolTable);
    typeCheckStatement(stmt->stmt.if_stmt->then, symbolTable);
    typeCheckStatement(stmt->stmt.if_stmt->else_stmt, symbolTable);
}

static void handleTypeCheckCompound(CStatement* stmt, SymbolTable* symbolTable) {
    typeCheckBlock(stmt->stmt.compound_stmt->block, symbolTable);
}

static void handleTypeCheckWhile(CStatement* stmt, SymbolTable* symbolTable) {
    typeCheckExpression(stmt->stmt.while_stmt->condition, symbolTable);
    typeCheckStatement(stmt->stmt.while_stmt->body, symbolTable);
}

static void handleTypeCheckDoWhile(CStatement* stmt, SymbolTable* symbolTable) {
    typeCheckStatement(stmt->stmt.do_while_stmt->body, symbolTable);
    typeCheckExpression(stmt->stmt.do_while_stmt->condition, symbolTable);
}

static void handleTypeCheckFor(CStatement* stmt, SymbolTable* symbolTable) {
    if (stmt->stmt.for_stmt->init && stmt->stmt.for_stmt->init->type == FOR_INIT_DECL) {
        typeCheckLocalVariableDeclaration(stmt->stmt.for_stmt->init->decl, symbolTable);
    } else if (stmt->stmt.for_stmt->init && stmt->stmt.for_stmt->init->type == FOR_INIT_EXP) {
        typeCheckExpression(stmt->stmt.for_stmt->init->exp, symbolTable);
    }
    typeCheckExpression(stmt->stmt.for_stmt->condition, symbolTable);
    typeCheckExpression(stmt->stmt.for_stmt->post, symbolTable);
    typeCheckStatement(stmt->stmt.for_stmt->body, symbolTable);
}

static void handleTypeCheckNoop(CStatement* stmt, SymbolTable* symbolTable) {
    /* nothing */
}

typedef void (*StmtTypeChecker)(CStatement*, SymbolTable*);

static const StmtTypeChecker stmtTypeCheckers[] = {
    [STMT_RETURN]     = handleTypeCheckReturn,
    [STMT_EXPRESSION] = handleTypeCheckExpression,
    [STMT_IF]         = handleTypeCheckIf,
    [STMT_COMPOUND]   = handleTypeCheckCompound,
    [STMT_WHILE]      = handleTypeCheckWhile,
    [STMT_DO_WHILE]   = handleTypeCheckDoWhile,
    [STMT_FOR]        = handleTypeCheckFor,
    [STMT_BREAK]      = handleTypeCheckNoop,
    [STMT_CONTINUE]   = handleTypeCheckNoop,
    [STMT_NULL]       = handleTypeCheckNoop,
};

void typeCheckStatement(CStatement* stmt, SymbolTable* symbolTable) {
    if (!stmt) return;
    StmtTypeChecker handler = stmtTypeCheckers[stmt->type];
    if (!handler) return;
    handler(stmt, symbolTable);
}
