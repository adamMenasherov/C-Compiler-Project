#include "FunctionTypeChecker.h"
#include "../../TypeChecker/TypeChecker.h"
#include "../../StatementTypeChecker/StatementTypeChecker.h"
#include "../../../../utils/SemanticUtils/SemanticUtils.h"
#include <stdio.h>
#include <stdlib.h>

static int areFunctionTypesCompatible(const CType* left, const CType* right) {
    if (!left || !right) return 0;
    if (left->kind != CTYPE_FUN || right->kind != CTYPE_FUN) return 0;
    if (!ctypeEqual(left->fun.ret, right->fun.ret)) return 0;
    if (left->fun.paramCnt != right->fun.paramCnt) return 0;
    for (int i = 0; i < left->fun.paramCnt; i++) {
        if (!ctypeEqual(left->fun.params[i], right->fun.params[i])) return 0;
    }
    return 1;
}

static void validateFunctionRedeclaration(CDeclaration* decl, IdentifierTypeInfo* existing,
                                           int requestedGlobal, int* alreadyDefined, int* global) {
    if (!existing->type || existing->type->kind != CTYPE_FUN ||
        !areFunctionTypesCompatible(existing->type, decl->decl.functionDecl.funcType)) {
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

static void insertFunctionParams(IdentifierArray* params, CType* funcType, SymbolTable* symbolTable) {
    for (int i = 0; i < IdentifierArray_size(params); i++) {
        char* param = IdentifierArray_get(params, i);
        CType* paramType = (funcType && i < funcType->fun.paramCnt) ? funcType->fun.params[i] : C_CreateType(CTYPE_INT);
        symbolTableInsert(symbolTable, param, paramType, 1,
            createIdentifierAttrs(IDENTIFIER_LOCAL_ATTR, 0, NULL, 0));
    }
}

void typeCheckFunctionDeclaration(CDeclaration* decl, SymbolTable* symbolTable) {
    if (!decl) return;

    int requestedGlobal = decl->decl.functionDecl.storageClass == STORAGE_CLASS_STATIC ? 0 : -1;
    int global = requestedGlobal == -1 ? 1 : requestedGlobal;
    int alreadyDefined = 0;

    if (symbolTableContains(symbolTable, decl->decl.functionDecl.identifier)) {
        IdentifierTypeInfo* existing = symbolTableLookup(symbolTable, decl->decl.functionDecl.identifier);
        validateFunctionRedeclaration(decl, existing, requestedGlobal, &alreadyDefined, &global);
    }

    symbolTableInsert(symbolTable, decl->decl.functionDecl.identifier,
        decl->decl.functionDecl.funcType,
        alreadyDefined || decl->decl.functionDecl.body != NULL,
        createIdentifierAttrs(IDENTIFIER_FUN_ATTR, global, NULL, alreadyDefined));

    insertFunctionParams(decl->decl.functionDecl.parameters, decl->decl.functionDecl.funcType, symbolTable);
    if (decl->decl.functionDecl.funcType)
        setCurrentFunctionReturnType(decl->decl.functionDecl.funcType->fun.ret);
    typeCheckBlock(decl->decl.functionDecl.body, symbolTable);
}
