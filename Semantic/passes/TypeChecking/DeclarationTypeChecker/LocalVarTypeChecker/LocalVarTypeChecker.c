#include "LocalVarTypeChecker.h"
#include "../../ExpressionTypeChecker/ExpressionTypeChecker.h"
#include "../../../../utils/SemanticUtils/SemanticUtils.h"
#include <stdio.h>
#include <stdlib.h>

static void handleExternLocalVar(CDeclaration* decl, SymbolTable* symbolTable) {
    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP) {
        fprintf(stderr, "Semantic Error: Local external variable '%s' cannot have an initializer\n",
            decl->decl.variableDecl.identifier);
        exit(1);
    }
    if (symbolTableContains(symbolTable, decl->decl.variableDecl.identifier)) {
        IdentifierTypeInfo* existing = symbolTableLookup(symbolTable, decl->decl.variableDecl.identifier);
        if (existing->type == TYPE_FUNCTION) {
            fprintf(stderr, "Semantic Error: Function '%s' redeclared as variable\n",
                decl->decl.variableDecl.identifier);
            exit(1);
        }
        return;
    }
    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, TYPE_INT, NULL, 1,
        createIdentifierAttrs(IDENTIFIER_STATIC_ATTR, 1, createIntInitialValue(INITIAL_NO_VALUE, 0), 0));
}

static initialValue* resolveStaticLocalInitValue(CDeclaration* decl) {
    if (decl->decl.variableDecl.declType == VAR_DECL_WITHOUT_EXP)
        return createInitialValue(INITIAL_WITH_VALUE, 0);

    if (decl->decl.variableDecl.varType &&
        decl->decl.variableDecl.varType->type == SPEC_INT &&
        decl->decl.variableDecl.exp &&
        decl->decl.variableDecl.exp->type == FACTOR_CONSTANT)
        return createInitialValue(INITIAL_WITH_VALUE, decl->decl.variableDecl.exp->exp.cnst->val);

    fprintf(stderr, "Semantic Error: Non-constant initializer for local static variable '%s'\n",
        fromTempToOrigin(decl->decl.variableDecl.identifier));
    exit(1);
}

static void handleStaticLocalVar(CDeclaration* decl, SymbolTable* symbolTable) {
    initialValue* initValue = resolveStaticLocalInitValue(decl);
    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, TYPE_INT, NULL, 0,
        createIdentifierAttrs(IDENTIFIER_STATIC_ATTR, 0, initValue, 0));
}

static void handlePlainLocalVar(CDeclaration* decl, SymbolTable* symbolTable) {
    symbolTableInsert(symbolTable, decl->decl.variableDecl.identifier, TYPE_INT, NULL, 1,
        createIdentifierAttrs(IDENTIFIER_LOCAL_ATTR, 0, NULL, 0));
    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP)
        typeCheckExpression(decl->decl.variableDecl.exp, symbolTable);
}

typedef void (*LocalVarHandler)(CDeclaration*, SymbolTable*);
static const LocalVarHandler localVarHandlers[] = {
    [SPEC_EXTERN] = handleExternLocalVar,
    [SPEC_STATIC] = handleStaticLocalVar,
    [SPEC_NULL]   = handlePlainLocalVar,
};

void typeCheckLocalVariableDeclaration(CDeclaration* decl, SymbolTable* symbolTable) {
    if (!decl) return;
    LocalVarHandler handler = localVarHandlers[decl->decl.variableDecl.storageClass];
    if (!handler) return;
    handler(decl, symbolTable);
}
