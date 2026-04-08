#include "semantic.h"
#include "../DataStructures/DynamicArray/DynamicArray.h"
#include "../Parser/TACKY/TACKYUtils/TACKYConstructors.h"
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
    resolveBlockWithLabeling(func->block);
    freeSemanticVariableMap(varMap);
}

void resolveBlockWithLabeling(CBlock* block) {
    if (!block) return;
    DArray_forEach(block->items, elem,
    {
        CBlockItem* blockItem = (CBlockItem*)elem;
        resolveBlockStatementsWithLabeling(blockItem);
    });
}


void resolveBlock(CBlock* block, SemanticVariableMap* varMap) {
    if (!block) return;
    DArray_forEach(block->items, elem,
    {
        CBlockItem* blockItem = (CBlockItem*)elem;
        resolveBlockItem(blockItem, varMap);
    });
}


void resolveBlockStatementsWithLabeling(CBlockItem* block)
{
    switch(block->type) {
        case BLOCK_ITEM_DECL:
            break;
        case BLOCK_ITEM_STMT:
            labelStatement(block->item.stmt, NULL);
            break;
    }
}

void labelStatement(CStatement* stmt, char* currentLabel) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_IF: {
            labelStatement(stmt->stmt.if_stmt->then, currentLabel);
            if (stmt->stmt.if_stmt->else_stmt) {
                labelStatement(stmt->stmt.if_stmt->else_stmt, currentLabel);
            }
            break;
        }
        case STMT_COMPOUND: {
            CBlock* block = stmt->stmt.compound_stmt->block;
            if (!block) break;
            DArray_forEach(block->items, elem,
            {
                CBlockItem* blockItem = (CBlockItem*)elem;
                if (blockItem && blockItem->type == BLOCK_ITEM_STMT) {
                    labelStatement(blockItem->item.stmt, currentLabel);
                }
            });
            break;
        }
        case STMT_FOR: {
            char* loopLabel = generateLoopName();
            stmt->stmt.for_stmt->identifier = strdup(loopLabel);
            labelStatement(stmt->stmt.for_stmt->body, loopLabel);
            break;
        }
            
        case STMT_WHILE: {
            char* whileLabel = generateLoopName();
            stmt->stmt.while_stmt->identifier = strdup(whileLabel);
            labelStatement(stmt->stmt.while_stmt->body, whileLabel);
            break;
        }
            
        case STMT_DO_WHILE: {
            char* doWhileLabel = generateLoopName();
            stmt->stmt.do_while_stmt->identifier = strdup(doWhileLabel);
            labelStatement(stmt->stmt.do_while_stmt->body, doWhileLabel);
            break;
        }
        case STMT_BREAK: {
            if (!currentLabel) {
                fprintf(stderr, "Semantic Error: 'break' statement not within a loop\n");
                exit(1);
            }
            stmt->stmt.break_stmt->identifier = strdup(currentLabel);
            break;
        }
        case STMT_CONTINUE: {
            if (!currentLabel) {
                fprintf(stderr, "Semantic Error: 'continue' statement not within a loop\n");
                exit(1);
            }
            stmt->stmt.continue_stmt->identifier = strdup(currentLabel);
            break;
        }

        default:
            break;
    }
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

    if (decl->declType == VAR_DECL_WITH_EXP && decl->exp) {
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
        case STMT_FOR: {
            SemanticVariableMap* newVarMap = copySemanticVariableMap(varMap);
            resolveForInit(stmt->stmt.for_stmt->init, newVarMap);
            resolveExpression(stmt->stmt.for_stmt->condition, newVarMap);
            resolveExpression(stmt->stmt.for_stmt->post, newVarMap);
            resolveStatement(stmt->stmt.for_stmt->body, newVarMap);
            freeSemanticVariableMap(newVarMap);
            break;
        }
        case STMT_WHILE: {
            SemanticVariableMap* newVarMap = copySemanticVariableMap(varMap);
            resolveExpression(stmt->stmt.while_stmt->condition, newVarMap);
            resolveStatement(stmt->stmt.while_stmt->body, newVarMap);
            freeSemanticVariableMap(newVarMap);
            break;
        }
        case STMT_DO_WHILE: {
            SemanticVariableMap* newVarMap = copySemanticVariableMap(varMap);
            resolveStatement(stmt->stmt.do_while_stmt->body, newVarMap);
            resolveExpression(stmt->stmt.do_while_stmt->condition, newVarMap);
            freeSemanticVariableMap(newVarMap);
            break;
        }
        case STMT_NULL:
            break;
        default: break;
    }
}


void resolveForInit(CForInit* init, SemanticVariableMap* varMap) {
    if (!init) return;
    switch (init->type) {
        case FOR_INIT_DECL:
            resolveDeclaration(init->decl, varMap);
            break;
        case FOR_INIT_EXP:
            resolveExpression(init->exp, varMap);
            break;
        case FOR_INIT_WITHOUT:
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