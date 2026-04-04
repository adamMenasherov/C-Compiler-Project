#include "semantic.h"
#include "Parser/TACKY/TACKY_AST.h"
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
    DArray_forEach(func->body, elem,
        {
            CBlockItem* blockItem = (CBlockItem*)elem;
            resolveBlockItem(blockItem, varMap);
        });
    freeSemanticVariableMap(varMap);
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
    
    if (semanticMapContainsKey(varMap, decl->identifier)) {
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
        case FACTOR_ASSIGNMENT:
            if (fact->exp.assignment->exp1->type != FACTOR_VAR) {
                fprintf(stderr, "Semantic Error: Left-hand side of assignment must be a variable\n");
                exit(1);
            }

            resolveExpression(fact->exp.assignment->exp1, varMap);
            resolveExpression(fact->exp.assignment->exp2, varMap);
            break;
    }
}

char* generateUniqueVariableName(char* baseName) {  
    char* uniqueName = malloc(strlen(baseName) + 20); 
    if (!uniqueName) return NULL;
    sprintf(uniqueName, "%s.%d", baseName, currGlobalInt++);
    return uniqueName;
}