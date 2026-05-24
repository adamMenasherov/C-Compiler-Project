#include "C-ASTNodeFree.h"
#include <stdlib.h>

static void C_freeIdentifierArray(IdentifierArray* identifiers) {
    if (!identifiers) return;
    IdentifierArray_free(identifiers);
}

static void C_freeForInit(CForInit* init) {
    if (!init) return;

    switch (init->type) {
        case FOR_INIT_DECL:
            C_freeDeclaration(init->decl);
            break;
        case FOR_INIT_EXP:
            C_freeFactor(init->exp);
            break;
        case FOR_INIT_WITHOUT:
            break;
    }

    free(init);
}

static void C_freeLoopStmt(CLoopStmt* loopStmt) {
    if (!loopStmt) return;
    free(loopStmt->identifier);
    free(loopStmt);
}

static void C_freeLoop(CLoop* loop) {
    if (!loop) return;
    C_freeFactor(loop->condition);
    C_freeStatement(loop->body);
    free(loop->identifier);
    free(loop);
}

static void C_freeForLoop(CForLoop* loop) {
    if (!loop) return;
    C_freeForInit(loop->init);
    C_freeFactor(loop->condition);
    C_freeFactor(loop->post);
    C_freeStatement(loop->body);
    free(loop->identifier);
    free(loop);
}

static void C_freeIf(CIf* ifNode) {
    if (!ifNode) return;
    C_freeFactor(ifNode->condition);
    C_freeStatement(ifNode->then);
    C_freeStatement(ifNode->else_stmt);
    free(ifNode);
}

static void C_freeCompound(CCompound* compound) {
    if (!compound) return;
    C_freeBlock(compound->block);
    free(compound);
}

static void C_freeVar(CVar* var) {
    if (!var) return;
    free(var->identifier);
    free(var);
}

static void C_freeUnary(CUnary* unary) {
    if (!unary) return;
    C_freeFactor(unary->exp);
    free(unary);
}

static void C_freeBinary(CBinary* binary) {
    if (!binary) return;
    C_freeFactor(binary->left);
    C_freeFactor(binary->right);
    free(binary);
}

static void C_freeAssignment(CAssignment* assignment) {
    if (!assignment) return;
    C_freeFactor(assignment->exp1);
    C_freeFactor(assignment->exp2);
    free(assignment);
}

static void C_freeConditional(CConditional* conditional) {
    if (!conditional) return;
    C_freeFactor(conditional->condition);
    C_freeFactor(conditional->then);
    C_freeFactor(conditional->else_stmt);
    free(conditional);
}

static void C_freeCast(CCast* castNode) {
    if (!castNode) return;
    C_freeFactor(castNode->exp);
    free(castNode);
}

static void C_freeFunctionCall(CFunctionCall* funcCall) {
    if (!funcCall) return;
    free(funcCall->identifier);
    if (funcCall->arguments) {
        for (int i = 0; i < ExpressionFactorArray_size(funcCall->arguments); i++) {
            CFactor* arg = ExpressionFactorArray_get(funcCall->arguments, i);
            C_freeFactor(arg);
        }
        ExpressionFactorArray_free(funcCall->arguments);
    }
    free(funcCall);
}

void C_freeProgram(CProgram* prog) {
    if (!prog) return;
    C_freeFunctions(prog->function_def);
    free(prog);
}

void C_freeFunctions(CDeclarationArray* functions) {
    if (!functions) return;

    for (int i = 0; i < CDeclarationArray_size(functions); i++) {
        CDeclaration* functionDecl = CDeclarationArray_get(functions, i);
        C_freeFunction(functionDecl);
    }

    CDeclarationArray_free(functions);
}

void C_freeFunction(CDeclaration* func) {
    C_freeDeclaration(func);
}

void C_freeDeclaration(CDeclaration* decl) {
    if (!decl) return;

    switch (decl->type) {
        case DECL_VAR:
            C_freeFactor(decl->decl.variableDecl.exp);
            break;
        case DECL_FUNC:
            free(decl->decl.functionDecl.identifier);
            C_freeIdentifierArray(decl->decl.functionDecl.parameters);
            C_freeBlock(decl->decl.functionDecl.body);
            break;
    }

    free(decl);
}

void C_freeStatement(CStatement* stmt) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_RETURN:
            C_freeReturn(stmt->stmt.ret);
            break;
        case STMT_EXPRESSION:
            C_freeFactor(stmt->stmt.exp);
            break;
        case STMT_IF:
            C_freeIf(stmt->stmt.if_stmt);
            break;
        case STMT_COMPOUND:
            C_freeCompound(stmt->stmt.compound_stmt);
            break;
        case STMT_BREAK:
            C_freeLoopStmt(stmt->stmt.break_stmt);
            break;
        case STMT_CONTINUE:
            C_freeLoopStmt(stmt->stmt.continue_stmt);
            break;
        case STMT_WHILE:
            C_freeLoop(stmt->stmt.while_stmt);
            break;
        case STMT_FOR:
            C_freeForLoop(stmt->stmt.for_stmt);
            break;
        case STMT_DO_WHILE:
            C_freeLoop(stmt->stmt.do_while_stmt);
            break;
        case STMT_NULL:
            break;
    }

    free(stmt);
}

void C_freeBlockItem(CBlockItem* item) {
    if (!item) return;

    switch (item->type) {
        case BLOCK_ITEM_DECL:
            C_freeDeclaration(item->item.decl);
            break;
        case BLOCK_ITEM_STMT:
            C_freeStatement(item->item.stmt);
            break;
    }

    free(item);
}

void C_freeBlock(CBlock* block) {
    if (!block) return;

    if (block->items) {
        for (int i = 0; i < BlockItemArray_size(block->items); i++) {
            CBlockItem* item = BlockItemArray_get(block->items, i);
            C_freeBlockItem(item);
        }
        BlockItemArray_free(block->items);
    }

    free(block);
}

void C_freeFactor(CFactor* factor) {
    if (!factor) return;

    switch (factor->type) {
        case FACTOR_CONSTANT:
            C_freeConstant(factor->exp.cnst);
            break;
        case FACTOR_UNARY:
            C_freeUnary(factor->exp.unary);
            break;
        case FACTOR_BINARY:
            C_freeBinary(factor->exp.binary);
            break;
        case FACTOR_VAR:
            C_freeVar(factor->exp.var);
            break;
        case FACTOR_ASSIGNMENT:
            C_freeAssignment(factor->exp.assignment);
            break;
        case FACTOR_CONDITIONAL:
            C_freeConditional(factor->exp.conditional);
            break;
        case FACTOR_CAST:
            C_freeCast(factor->exp.cast);
            break;
        case FACTOR_FUNCTION_CALL:
            C_freeFunctionCall(factor->exp.funcCall);
            break;
    }

    free(factor);
}

void C_freeReturn(CReturn* returnNode) {
    if (!returnNode) return;
    C_freeFactor(returnNode->exp);
    free(returnNode);
}

void C_freeConstant(CConstant* constant) {
    free(constant);
}
