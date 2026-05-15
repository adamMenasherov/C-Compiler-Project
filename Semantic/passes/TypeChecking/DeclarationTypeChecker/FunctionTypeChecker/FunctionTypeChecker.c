#include "FunctionTypeChecker.h"
#include "../../TypeChecker/TypeChecker.h"
#include "../../../../utils/SemanticUtils/SemanticUtils.h"
#include <stdio.h>
#include <stdlib.h>

static int areFunctionTypesCompatible(const CFuncType* left, const CFuncType* right) {
    int idx = 0;
    if (!left || !right) return 0;
    if (!left->func.ret || !right->func.ret) return 0;
    if (left->func.ret->type != right->func.ret->type) return 0;

    while (idx < MAX_PARAMS) {
        CFuncType* leftParam  = left->func.params[idx];
        CFuncType* rightParam = right->func.params[idx];
        if (!leftParam && !rightParam) return 1;
        if (!leftParam || !rightParam) return 0;
        if (leftParam->type != rightParam->type) return 0;
        idx++;
    }
    return 1;
}

static void validateFunctionRedeclaration(CDeclaration* decl, IdentifierTypeInfo* existing,
                                           int requestedGlobal, int* alreadyDefined, int* global) {
    if (existing->type != TYPE_FUNCTION ||
        !areFunctionTypesCompatible(existing->funcInfo.funcType, decl->decl.functionDecl.funcType)) {
        fprintf(stderr, "Semantic Error: Incompatible function declarations for '%s'\n",
            decl->decl.functionDecl.identifier);
        exit(1);
    }
    if (requestedGlobal != -1 && existing->attrs->global != requestedGlobal) {
        fprintf(stderr, "Semantic Error: Conflicting function linkage for '%s'\n",
            decl->decl.functionDecl.identifier);
        exit(1);
    }
    *alreadyDefined = existing->funcInfo.isDefined;
    if (*alreadyDefined && decl->decl.functionDecl.body) {
        fprintf(stderr, "Semantic Error: Function '%s' is defined more than once\n",
            decl->decl.functionDecl.identifier);
        exit(1);
    }
    *global = (requestedGlobal == -1) ? existing->attrs->global : requestedGlobal;
}

static void insertFunctionParams(IdentifierArray* params, SymbolTable* symbolTable) {
    for (int i = 0; i < IdentifierArray_size(params); i++) {
        char* param = IdentifierArray_get(params, i);
        symbolTableInsert(symbolTable, param, TYPE_INT, NULL, 1,
            createIdentifierAttrs(IDENTIFIER_LOCAL_ATTR, 0, NULL, 0));
    }
}

void typeCheckFunctionDeclaration(CDeclaration* decl, SymbolTable* symbolTable) {
    if (!decl) return;

    int requestedGlobal = decl->decl.functionDecl.storageClass == SPEC_STATIC ? 0 : -1;
    int global          = requestedGlobal == -1 ? 1 : requestedGlobal;
    int alreadyDefined  = 0;

    if (symbolTableContains(symbolTable, decl->decl.functionDecl.identifier)) {
        IdentifierTypeInfo* existing = symbolTableLookup(symbolTable, decl->decl.functionDecl.identifier);
        validateFunctionRedeclaration(decl, existing, requestedGlobal, &alreadyDefined, &global);
    }

    symbolTableInsert(symbolTable, decl->decl.functionDecl.identifier,
        TYPE_FUNCTION, decl->decl.functionDecl.funcType,
        alreadyDefined || decl->decl.functionDecl.body != NULL,
        createIdentifierAttrs(IDENTIFIER_FUN_ATTR, global, NULL, alreadyDefined));

    insertFunctionParams(decl->decl.functionDecl.parameters, symbolTable);
    typeCheckBlock(decl->decl.functionDecl.body, symbolTable);
}
