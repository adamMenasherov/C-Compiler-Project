#include "semantic.h"
#include "../DataStructures/DynamicArray/DynamicArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void resolveAST(AST* ast) {
    if (!ast) return;
    resolveProgram(ast->prog);
}

void resolveProgram(CProgram* prog) {
    if (!prog) return;
    resolveFunction(prog->function_def);
}
void resolveFunction(CFunction* func) {
    if (!func) return;
    SemanticVariableMap* varMap = createSemanticVariableMap();
    resolveBlock(func->block, varMap);
    freeSemanticVariableMap(varMap);
}

void resolveBlock(CBlock* block, SemanticVariableMap* varMap) {
    if (!block) return;
    DArray_forEach(block->items, elem,
    {
        CBlockItem* blockItem = (CBlockItem*)elem;
        resolveBlockItem(blockItem, varMap);
    });
}

void resolveBlockItem(CBlockItem* blockItem, SemanticVariableMap* varMap) {
    if (!blockItem) return;
    switch (blockItem->type) {
        case BLOCK_ITEM_DECL:
            resolveDeclaration(blockItem->item.decl, varMap);
            break;
        case BLOCK_ITEM_STMT:
            resolveStatement(blockItem->item.stmt, varMap);
            break;
    }
}

void resolveDeclaration(CDeclaration* decl, SemanticVariableMap* varMap) {
    if (!decl) return;
    
    if (semanticMapContainsKey(varMap, decl->identifier) && isFromCurrentBlock(varMap, decl->identifier)) {
        fprintf(stderr, "Semantic Error: Variable '%s' redeclared\n", decl->identifier);
        exit(1);
    }

    char* uniqueName = generateUniqueVariableName(decl->identifier);
    if (!uniqueName) {
        fprintf(stderr, "Semantic Error: Failed to generate unique variable name for '%s'\n", decl->identifier);
        exit(1);
    }
    semanticMapPut(varMap, decl->identifier, uniqueName);
    decl->identifier = uniqueName;

    if (decl->declType == DECL_WITH_EXP && decl->exp) {
        resolveExpression(decl->exp, varMap);
    }
}

void resolveStatement(CStatement* stmt, SemanticVariableMap* varMap) {
    if (!stmt) return;
    switch(stmt->type) {
        case STMT_EXPRESSION:
            resolveExpression(stmt->stmt.exp, varMap);
            break;
        case STMT_RETURN:
            resolveExpression(stmt->stmt.ret->exp, varMap);
            break;
        case STMT_IF: {
            resolveExpression(stmt->stmt.if_stmt->condition, varMap);
            resolveStatement(stmt->stmt.if_stmt->then, varMap);
            if (stmt->stmt.if_stmt->else_stmt) {
                resolveStatement(stmt->stmt.if_stmt->else_stmt, varMap);
            }
            break;
        }
        case STMT_COMPOUND: {
            SemanticVariableMap* newVarMap = copySemanticVariableMap(varMap);
            resolveBlock(stmt->stmt.compound_stmt->block, newVarMap);
            freeSemanticVariableMap(newVarMap);
            break;
        }
            
        case STMT_NULL:
            break;
    }
}
void resolveExpression(CFactor* fact, SemanticVariableMap* varMap) {
    if (!fact) return;

    switch (fact->type) {
        case FACTOR_CONSTANT:
            break;
        case FACTOR_UNARY:
            if ((fact->exp.unary->type == UNARY_INCREMENT_PREFIX ||
                 fact->exp.unary->type == UNARY_INCREMENT_POSTFIX ||
                 fact->exp.unary->type == UNARY_DECREMENT_PREFIX ||
                 fact->exp.unary->type == UNARY_DECREMENT_POSTFIX) &&
                fact->exp.unary->exp->type != FACTOR_VAR) {
                fprintf(stderr, "Semantic Error: Operand of %s must be a variable\n",
                    (fact->exp.unary->type == UNARY_INCREMENT_PREFIX ||
                     fact->exp.unary->type == UNARY_INCREMENT_POSTFIX) ? "++" : "--");
                exit(1);
            }
            resolveExpression(fact->exp.unary->exp, varMap);
            break;
        case FACTOR_BINARY:
            resolveExpression(fact->exp.binary->left, varMap);
            resolveExpression(fact->exp.binary->right, varMap);
            break;
        case FACTOR_VAR: {
            char* uniqueName = semanticMapGet(varMap, fact->exp.var->identifier);
            if (!uniqueName) {
                fprintf(stderr, "Semantic Error: Undeclared variable '%s'\n", fact->exp.var->identifier);
                exit(1);
            }
            fact->exp.var->identifier = uniqueName;
            break;
        }
        case FACTOR_ASSIGNMENT: {
            if (fact->exp.assignment->exp1->type != FACTOR_VAR) {
                fprintf(stderr, "Semantic Error: Left-hand side of assignment must be a variable\n");
                exit(1);
            }

            resolveExpression(fact->exp.assignment->exp1, varMap);
            resolveExpression(fact->exp.assignment->exp2, varMap);
            break;
        }
        case FACTOR_CONDITIONAL:
            resolveExpression(fact->exp.conditional->condition, varMap);
            resolveExpression(fact->exp.conditional->then, varMap);
            resolveExpression(fact->exp.conditional->else_stmt, varMap);
            break;
    }
}

char* generateUniqueVariableName(char* baseName) {  
    char* uniqueName = malloc(strlen(baseName) + 20); 
    if (!uniqueName) return NULL;
    sprintf(uniqueName, "%s.%d", baseName, currGlobalInt++);
    return uniqueName;
}