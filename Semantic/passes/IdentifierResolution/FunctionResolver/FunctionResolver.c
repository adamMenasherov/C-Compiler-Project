#include "FunctionResolver.h"
#include "../BlockResolver/BlockResolver.h"
#include "../../LoopLabeling/LoopLabeler/LoopLabeler.h"
#include "../../../utils/ScopeUtils/ScopeUtils.h"
#include "../../../utils/SemanticUtils/SemanticUtils.h"
#include <stdio.h>
#include <stdlib.h>

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

    if (declLevel == BLOCK_LEVEL && func->decl.functionDecl.body) {
        fprintf(stderr, "Semantic Error: Nested function definition is not allowed ('%s')\n", func->decl.functionDecl.identifier);
        exit(1);
    }

    semanticMapPut(varMap, func->decl.functionDecl.identifier, func->decl.functionDecl.identifier, 1, 1);
    if (!symbolTableContains(symbolTable, func->decl.functionDecl.identifier)) {
        int global = (func->decl.functionDecl.storageClass == SPEC_STATIC) ? 0 : 1;
        symbolTableInsert(
            symbolTable,
            func->decl.functionDecl.identifier,
            TYPE_FUNCTION,
            func->decl.functionDecl.funcType,
            0,
            createIdentifierAttrs(IDENTIFIER_FUN_ATTR, global, NULL, 0)
        );
    }
    SemanticIdentifierMap* newVarMap = copySemanticIdentifierMap(varMap);
    markAllEntriesOutOfCurrentScope(newVarMap);
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
        symbolTableInsert(symbolTable, uniqueParamName, TYPE_INT, NULL, 1,
            createIdentifierAttrs(IDENTIFIER_LOCAL_ATTR, 0, NULL, 0));
    }
}
