#include "FileScopeVarTypeChecker.h"
#include "../../../../utils/SemanticUtils/SemanticUtils.h"
#include <stdio.h>
#include <stdlib.h>

static void validateFileScopeType(CDeclaration* decl) {
    if (!decl->decl.variableDecl.varType ||
        !isBasicType(decl->decl.variableDecl.varType->type) ||
        !decl->decl.variableDecl.exp ||
        decl->decl.variableDecl.exp->type != FACTOR_CONSTANT) {
        fprintf(stderr, "Semantic Error: Only int variables can be initialized at file scope. Variable '%s' has invalid type\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
}

static initialValue* resolveFileScopeInitValue(CDeclaration* decl) {
    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP)
        return createInitialValue(INITIAL_WITH_VALUE, decl->decl.variableDecl.exp->exp.cnst->val);
    if (decl->decl.variableDecl.storageClass == SPEC_EXTERN)
        return createInitialValue(INITIAL_NO_VALUE, 0);
    return createInitialValue(INITIAL_TENTATIVE, 0);
}

static int resolveFileScopeLinkage(CDeclaration* decl, IdentifierTypeInfo* existing, int requestedGlobal) {
    if (!existing) return requestedGlobal;
    if (existing->type == TYPE_FUNCTION) {
        fprintf(stderr, "Semantic Error: Function '%s' redeclared as variable\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
    if (decl->decl.variableDecl.storageClass == SPEC_EXTERN)
        return existing->attrs->global;
    if (existing->attrs->global != requestedGlobal) {
        fprintf(stderr, "Semantic Error: Conflicting variable linkage for variable '%s'\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
    return requestedGlobal;
}

static initialValue* resolveFileScopeExistingInit(CDeclaration* decl, IdentifierTypeInfo* existing, initialValue* initValue) {
    if (!existing) return initValue;
    if (existing->attrs->attrs.staticAttr.initValue->type != INITIAL_WITH_VALUE) return initValue;
    if (initValue && initValue->type == INITIAL_WITH_VALUE) {
        fprintf(stderr, "Semantic Error: Multiple initializers for variable '%s'\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
    return existing->attrs->attrs.staticAttr.initValue;
}

void typeCheckFileScopeVariableDeclaration(CDeclaration* decl, SymbolTable* symbolTable) {
    if (!decl || decl->type != DECL_VAR) return;
    validateFileScopeType(decl);

    initialValue* initValue = resolveFileScopeInitValue(decl);
    int global = decl->decl.variableDecl.storageClass == SPEC_STATIC ? 0 : 1;
    IdentifierTypeInfo* existing = symbolTableContains(symbolTable, decl->decl.variableDecl.identifier)
        ? symbolTableLookup(symbolTable, decl->decl.variableDecl.identifier)
        : NULL;

    global = resolveFileScopeLinkage(decl, existing, global);
    initValue = resolveFileScopeExistingInit(decl, existing, initValue);

    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, TYPE_INT, NULL, global,
        createIdentifierAttrs(IDENTIFIER_STATIC_ATTR, global, initValue, 0));
}
