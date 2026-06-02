#include "VariableResolver.h"
#include "../ExpressionResolver/ExpressionResolver.h"
#include "../../../utils/SemanticUtils/SemanticUtils.h"
#include <stdio.h>
#include <stdlib.h>

void resolveFileScopeVarDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap) {
    semanticMapPut(varMap, decl->decl.variableDecl.identifier, decl->decl.variableDecl.identifier, 1, 1);
}

void resolveLocalVarDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (semanticMapContainsKey(varMap, decl->decl.variableDecl.identifier)) {
        MapEntry* entry = getSemanticMapEntry(varMap, decl->decl.variableDecl.identifier);
        if (entry->isInScope) {
            if (decl->decl.variableDecl.storageClass == STORAGE_CLASS_EXTERN) {
                if (!entry->hasExternalLinkage) {
                    fprintf(stderr, "Semantic Error: Variable '%s' - conflicting local declaration\n",
                        decl->decl.variableDecl.identifier);
                    exit(1);
                }
                return;
            }
            if (!(entry->hasExternalLinkage && decl->decl.variableDecl.storageClass == STORAGE_CLASS_EXTERN)) {
                fprintf(stderr, "Semantic Error: Variable '%s' - conflicting local declaration\n",
                    decl->decl.variableDecl.identifier);
                exit(1);
            }
        }
    }

    if (decl->decl.variableDecl.storageClass == STORAGE_CLASS_EXTERN) {
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
    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP) {
        resolveExpression(decl->decl.variableDecl.exp, varMap, symbolTable);
    }
}
