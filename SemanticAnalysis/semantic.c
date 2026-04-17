#include "semantic.h"
#include "../DataStructures/DynamicArray/DynamicArray.h"
#include "../DataStructures/DynamicArray/Wrappers/BlockItemArrayWrapper.h"
#include "../Parser/generateUtils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int isIncrementDecrementOpIncludingFix(unaryType type);


SymbolTable* resolveAST(AST* ast) {
    if (!ast) return NULL;
    SymbolTable* symbolTable = createSymbolTable();
    resolveProgram(ast->prog, symbolTable);
    return symbolTable;
}

void resolveProgram(CProgram* prog, SymbolTable* symbolTable) {
    if (!prog) return;
    SemanticIdentifierMap* varMap = createSemanticIdentifierMap();
    resolveDeclarations(prog->function_def, varMap, symbolTable);
    freeSemanticIdentifierMap(varMap);
}

void resolveDeclarations(CDeclarationArray* declarations, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!declarations) return;
    for (int i = 0; i < declarations->size; i++) {
        CDeclaration* declaration = (CDeclaration*)declarations->data[i];
        resolveDeclaration(declaration, varMap, symbolTable, TOP_LEVEL);
        switch(declaration->type) {
            case DECL_VAR:
                typeCheckVariableDeclaration(declaration, symbolTable);
                break;
            case DECL_FUNC:
                typeCheckFunctionDeclaration(declaration, symbolTable);
                break;
        }
    }
}

void resolveFunctionDeclaration(CDeclaration* func, SemanticIdentifierMap* varMap, SymbolTable* symbolTable, level declLevel) {
    char* uniqueName = semanticMapGet(varMap, func->decl.functionDecl.identifier);
    if (uniqueName) {
        MapEntry* entry = getSemanticMapEntry(varMap, func->decl.functionDecl.identifier);
        if (entry->isInScope && !entry->hasExternalLinkage) {
            fprintf(stderr, "Semantic Error: Function '%s' with static linkage redeclared\n", func->decl.functionDecl.identifier);
            exit(1);
        }
    }

    if (declLevel == BLOCK_LEVEL && func->decl.functionDecl.storageClass == SPEC_STATIC) {
        fprintf(stderr, "Semantic Error: Function '%s' with static storage class cannot be declared at block scope\n", func->decl.functionDecl.identifier);
        exit(1); 
    }

    semanticMapPut(varMap, func->decl.functionDecl.identifier, func->decl.functionDecl.identifier, 1, 1);
    if (!symbolTableContains(symbolTable, func->decl.functionDecl.identifier)) {
        symbolTableInsert(
            symbolTable,
            func->decl.functionDecl.identifier,
            TYPE_FUNCTION,
            IdentifierArray_size(func->decl.functionDecl.parameters),
            0
        );
    }
    SemanticIdentifierMap* newVarMap = copySemanticIdentifierMap(varMap);
    // Checking whether a parameter name conflicts with an existing variable in the function parameter list
    resolveParams(func->decl.functionDecl.parameters, newVarMap, symbolTable);     
    resolveBlock(func->decl.functionDecl.body, newVarMap, symbolTable);
    resolveBlockWithLabeling(func->decl.functionDecl.body);
    freeSemanticIdentifierMap(newVarMap);
}


void resolveParams(IdentifierArray* params, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    for (int i = 0; i < IdentifierArray_size(params); i++) {
        char* param = IdentifierArray_get(params, i);
        char* uniqueParamName = generateUniqueVariableName(param);
        if (!uniqueParamName) {
            fprintf(stderr, "Semantic Error: Failed to generate unique variable name for parameter '%s'\n", param);
            exit(1);
        }
        MapEntry* varInMap = getSemanticMapEntry(varMap, param);
        if (varInMap && varInMap->isInScope) {
            fprintf(stderr, "Semantic Error: Parameter '%s' conflicts with an existing variable in the function parameter list\n", param);
            exit(1);
        }
        semanticMapPut(varMap, param, uniqueParamName, 1, 0);
        IdentifierArray_set(params, i, uniqueParamName);
        symbolTableInsert(symbolTable, uniqueParamName, TYPE_INT, 0, 1); 
    }
}

void resolveBlockWithLabeling(CBlock* block) {
    if (!block) return;
    DArray_forEach(block->items, elem,
    {
        CBlockItem* blockItem = (CBlockItem*)elem;
        resolveBlockStatementsWithLabeling(blockItem);
    });
}


void resolveBlock(CBlock* block, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!block) return;

    for (int i = 0; i < IdentifierArray_size(block->items); i++) {
        CBlockItem* blockItem = (CBlockItem*)IdentifierArray_get(block->items, i);
        resolveBlockItem(blockItem, varMap, symbolTable);
    }
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

void resolveBlockItem(CBlockItem* blockItem, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!blockItem) return;
    switch (blockItem->type) {
        case BLOCK_ITEM_DECL:
            resolveDeclaration(blockItem->item.decl, varMap, symbolTable, BLOCK_LEVEL);
            break;
        case BLOCK_ITEM_STMT:
            resolveStatement(blockItem->item.stmt, varMap, symbolTable);
            break;
    }
}

void resolveDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable, level declLevel) {
    if (!decl) return;

    switch(declLevel) {
        case TOP_LEVEL:
            switch(decl->type) {
                case DECL_VAR:
                    resolveFileScopeVarDeclaration(decl, varMap);
                    break;
                case DECL_FUNC:
                    resolveFunctionDeclaration(decl, varMap, symbolTable, declLevel);
                    break;
            }
            break;
        case BLOCK_LEVEL:
            switch(decl->type) {
                case DECL_VAR:
                    resolveLocalVarDeclaration(decl, varMap);
                    break;
                case DECL_FUNC:
                    resolveFunctionDeclaration(decl, varMap, symbolTable, declLevel);
                    break;
            }
            break;
    }
}

void resolveVarDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!decl) return;
    
    if (semanticMapContainsKey(varMap, decl->decl.variableDecl.identifier) && 
            isFromCurrentBlock(varMap, decl->decl.variableDecl.identifier)) {
        fprintf(stderr, "Semantic Error: Variable '%s' redeclared\n", decl->decl.variableDecl.identifier);
        exit(1);
    }

    char* uniqueName = generateUniqueVariableName(decl->decl.variableDecl.identifier);
    if (!uniqueName) {
        fprintf(stderr, "Semantic Error: Failed to generate unique variable name for '%s'\n", decl->decl.variableDecl.identifier);
        exit(1);
    }
    semanticMapPut(varMap, decl->decl.variableDecl.identifier, uniqueName, 1, 0);
    decl->decl.variableDecl.identifier = uniqueName;
    symbolTableInsert(symbolTable, uniqueName, TYPE_INT, 0, 1);

    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP && decl->decl.variableDecl.exp) {
        resolveExpression(decl->decl.variableDecl.exp, varMap, symbolTable);
    }
}


void resolveStatement(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!stmt) return;
    switch(stmt->type) {
        case STMT_EXPRESSION:
            resolveExpression(stmt->stmt.exp, varMap, symbolTable);
            break;
        case STMT_RETURN:
            resolveExpression(stmt->stmt.ret->exp, varMap, symbolTable);
            break;
        case STMT_IF: {
            resolveExpression(stmt->stmt.if_stmt->condition, varMap, symbolTable);
            resolveStatement(stmt->stmt.if_stmt->then, varMap, symbolTable);
            if (stmt->stmt.if_stmt->else_stmt) {
                resolveStatement(stmt->stmt.if_stmt->else_stmt, varMap, symbolTable);
            }
            break;
        }
        case STMT_COMPOUND: {
            SemanticIdentifierMap* newVarMap = copySemanticIdentifierMap(varMap);
            resolveBlock(stmt->stmt.compound_stmt->block, newVarMap, symbolTable);
            freeSemanticIdentifierMap(newVarMap);
            break;
        }
        case STMT_FOR: {
            SemanticIdentifierMap* newVarMap = copySemanticIdentifierMap(varMap);
            resolveForInit(stmt->stmt.for_stmt->init, newVarMap, symbolTable);
            resolveExpression(stmt->stmt.for_stmt->condition, newVarMap, symbolTable);
            resolveExpression(stmt->stmt.for_stmt->post, newVarMap, symbolTable);
            resolveStatement(stmt->stmt.for_stmt->body, newVarMap, symbolTable);
            freeSemanticIdentifierMap(newVarMap);
            break;
        }
        case STMT_WHILE: {
            SemanticIdentifierMap* newVarMap = copySemanticIdentifierMap(varMap);
            resolveExpression(stmt->stmt.while_stmt->condition, newVarMap, symbolTable);
            resolveStatement(stmt->stmt.while_stmt->body, newVarMap, symbolTable);
            freeSemanticIdentifierMap(newVarMap);
            break;
        }
        case STMT_DO_WHILE: {
            SemanticIdentifierMap* newVarMap = copySemanticIdentifierMap(varMap);
            resolveStatement(stmt->stmt.do_while_stmt->body, newVarMap, symbolTable);
            resolveExpression(stmt->stmt.do_while_stmt->condition, newVarMap, symbolTable);
            freeSemanticIdentifierMap(newVarMap);
            break;
        }
        case STMT_NULL:
            break;
        default: break;
    }
}


void resolveForInit(CForInit* init, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!init) return;
    switch (init->type) {
        case FOR_INIT_DECL:
            resolveVarDeclaration(init->decl, varMap, symbolTable);
            typeCheckVariableDeclaration(init->decl, symbolTable);
            break;
        case FOR_INIT_EXP:
            resolveExpression(init->exp, varMap, symbolTable);
            break;
        case FOR_INIT_WITHOUT:
            break;
    }
}


void resolveExpression(CFactor* fact, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!fact) return;

    switch (fact->type) {
        case FACTOR_CONSTANT:
            break;
        case FACTOR_UNARY:
            if ((isIncrementDecrementOpIncludingFix(fact->exp.unary->type)) &&
                fact->exp.unary->exp->type != FACTOR_VAR) {
                fprintf(stderr, "Semantic Error: Operand of %s must be a variable\n",
                    (fact->exp.unary->type == UNARY_INCREMENT_PREFIX ||
                     fact->exp.unary->type == UNARY_INCREMENT_POSTFIX) ? "++" : "--");
                exit(1);
            }
            resolveExpression(fact->exp.unary->exp, varMap, symbolTable);
            break;
        case FACTOR_BINARY:
            resolveExpression(fact->exp.binary->left, varMap, symbolTable);
            resolveExpression(fact->exp.binary->right, varMap, symbolTable);
            break;
        case FACTOR_VAR: {
            char* uniqueName = semanticMapGet(varMap, fact->exp.var->identifier);
            if (!uniqueName) {
                fprintf(stderr, "Semantic Error: Undeclared variable '%s'\n", fact->exp.var->identifier);
                exit(1);
            }
            fact->exp.var->identifier = uniqueName;
            typeCheckExpression(fact, symbolTable);
            break;
        }
        case FACTOR_ASSIGNMENT: {
            if (fact->exp.assignment->exp1->type != FACTOR_VAR) {
                fprintf(stderr, "Semantic Error: Left-hand side of assignment must be a variable\n");
                exit(1);
            }

            resolveExpression(fact->exp.assignment->exp1, varMap, symbolTable);
            resolveExpression(fact->exp.assignment->exp2, varMap, symbolTable);
            break;
        }
        case FACTOR_CONDITIONAL: {
            resolveExpression(fact->exp.conditional->condition, varMap, symbolTable);
            resolveExpression(fact->exp.conditional->then, varMap, symbolTable);
            resolveExpression(fact->exp.conditional->else_stmt, varMap, symbolTable);
            break;
        }

        case FACTOR_FUNCTION_CALL: {
            char* uniqueName = semanticMapGet(varMap, fact->exp.funcCall->identifier);
            if (!uniqueName) {
                fprintf(stderr, "Semantic Error: Undeclared function '%s'\n", fact->exp.funcCall->identifier);
                exit(1);
            }
            fact->exp.funcCall->identifier = uniqueName;
            ExpressionFactorArray* args = fact->exp.funcCall->arguments;
            if (args) {
                for (int i = 0; i < args->size; i++) {
                    CFactor* arg = (CFactor*)args->data[i];
                    resolveExpression(arg, varMap, symbolTable);
                }
            }
            typeCheckExpression(fact, symbolTable);
            break;
        }
            
    }
}

void resolveFileScopeVarDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap) {
    semanticMapPut(varMap, decl->decl.variableDecl.identifier, decl->decl.variableDecl.identifier, 1, 1);
}

void resolveLocalVarDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap) {
    if (semanticMapContainsKey(varMap, decl->decl.variableDecl.identifier)) {
        MapEntry* entry = getSemanticMapEntry(varMap, decl->decl.variableDecl.identifier);
        if (entry->isInScope) {
            if (!(entry->hasExternalLinkage && 
                    decl->decl.variableDecl.storageClass == SPEC_EXTERN)) {
                    fprintf(stderr, "Semantic Error: Variable '%s' - conflicting local declaration\n", 
                        decl->decl.variableDecl.identifier);
                    exit(1);
            }
        }
    }

    if (decl->decl.variableDecl.storageClass == SPEC_EXTERN) {
        semanticMapPut(varMap, decl->decl.variableDecl.identifier, decl->decl.variableDecl.identifier, 1, 1);
        return;
    }
    else {
        char* uniqueName = generateUniqueVariableName(decl->decl.variableDecl.identifier);
        if (!uniqueName) {
            fprintf(stderr, "Semantic Error: Failed to generate unique variable name for '%s'\n", decl->decl.variableDecl.identifier);
            exit(1);
        }
        semanticMapPut(varMap, decl->decl.variableDecl.identifier, uniqueName, 1, 0);
        decl->decl.variableDecl.identifier = uniqueName;
    }    
}


void typeCheckVariableDeclaration(CDeclaration* decl, SymbolTable* symbolTable) {
    if (!decl) return;
    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, TYPE_INT, 0, 1);

    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP) {
        typeCheckExpression(decl->decl.variableDecl.exp, symbolTable);
    }
}

void typeCheckFunctionDeclaration(CDeclaration* decl, SymbolTable* symbolTable) {
    IdentifierTypeInfo* existing;
    int alreadyDefined = 0;
    if (!decl) return;

    IdentifierArray* params = decl->decl.functionDecl.parameters;
    size_t paramCount = IdentifierArray_size(params);

    if (symbolTableContains(symbolTable, decl->decl.functionDecl.identifier)) {
        existing = symbolTableLookup(symbolTable, decl->decl.functionDecl.identifier);
        if (existing->type != TYPE_FUNCTION || existing->funcInfo.paramCount != paramCount) // Making sure the signature matches if the function is already declared
        {
            fprintf(stderr, "Semantic Error: Incompatible function declarations for '%s'\n", decl->decl.functionDecl.identifier);
            exit(1);
        }
        alreadyDefined = existing->funcInfo.isDefined;
        if (alreadyDefined && decl->decl.functionDecl.body) {
            fprintf(stderr, "Semantic Error: Function '%s' is defined more than once\n", decl->decl.functionDecl.identifier);
            exit(1);
        }
    }

    symbolTableInsert(symbolTable, decl->decl.functionDecl.identifier, 
        TYPE_FUNCTION, paramCount, alreadyDefined || decl->decl.functionDecl.body != NULL);
    
    for (int i = 0; i < IdentifierArray_size(params); i++) {
        char* param = IdentifierArray_get(params, i);
        symbolTableInsert(symbolTable, param, TYPE_INT, 0, 1);
    }

    typeCheckBlock(decl->decl.functionDecl.body, symbolTable);
}

void typeCheckExpression(CFactor* expr, SymbolTable* symbolTable) {
    if (!expr) return;
    switch (expr->type) {
        case FACTOR_FUNCTION_CALL: {
            IdentifierTypeInfo* info = symbolTableLookup(symbolTable, expr->exp.funcCall->identifier);
            if (!info) {
                fprintf(stderr, "Semantic Error: Undeclared function '%s'\n", expr->exp.funcCall->identifier);
                exit(1);
            }
            if (info->type != TYPE_FUNCTION) {
                fprintf(stderr, "Semantic Error: '%s' is not a function\n", expr->exp.funcCall->identifier);
                exit(1);
            }
            ExpressionFactorArray* args = expr->exp.funcCall->arguments;
            if (IdentifierArray_size(args) != info->funcInfo.paramCount) {
                fprintf(stderr, "Semantic Error: Function '%s' called with incorrect number of arguments\n", expr->exp.funcCall->identifier);
                exit(1);
            }
            for (int i = 0; i < args->size; i++) {
                CFactor* arg = (CFactor*)args->data[i];
                typeCheckExpression(arg, symbolTable);
            }
            break;
        }

        case FACTOR_VAR: {
            IdentifierTypeInfo* info = symbolTableLookup(symbolTable, expr->exp.var->identifier);
            if (!info) {
                fprintf(stderr, "Semantic Error: Undeclared variable '%s'\n", expr->exp.var->identifier);
                exit(1);
            }
            if (info->type != TYPE_INT) {
                fprintf(stderr, "Semantic Error: '%s' is not a variable\n", expr->exp.var->identifier);
                exit(1);
            }
            break;
        }
        default: return;
    }
}

void typeCheckBlock(CBlock* block, SymbolTable* symbolTable) {
    if (!block) return;
    for (int i = 0; i < BlockItemArray_size(block->items); i++) {
        CBlockItem* blockItem = (CBlockItem*)BlockItemArray_get(block->items, i);
        if (blockItem->type == BLOCK_ITEM_DECL) {
            switch (blockItem->item.decl->type) {
                case DECL_VAR:
                    typeCheckVariableDeclaration(blockItem->item.decl, symbolTable);
                    break;
                case DECL_FUNC:
                    typeCheckFunctionDeclaration(blockItem->item.decl, symbolTable);
                    break;
            }
        }
    }
}

char* generateUniqueVariableName(char* baseName) {  
    char* uniqueName = malloc(strlen(baseName) + 20); 
    if (!uniqueName) return NULL;
    sprintf(uniqueName, "%s.%d", baseName, currGlobalInt++);
    return uniqueName;
}

static int isIncrementDecrementOpIncludingFix(unaryType type) {
    return type == UNARY_INCREMENT_PREFIX || type == UNARY_INCREMENT_POSTFIX ||
           type == UNARY_DECREMENT_PREFIX || type == UNARY_DECREMENT_POSTFIX;
}