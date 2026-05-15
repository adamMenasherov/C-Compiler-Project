#include "IdentifierResolver.h"
#include "../VariableResolver/VariableResolver.h"
#include "../FunctionResolver/FunctionResolver.h"
#include "../../TypeChecking/DeclarationTypeChecker/DeclarationTypeChecker.h"
#include <stdlib.h>
#include <stdio.h>

/* ---- Top-level declaration handlers ---- */

static void handleTopLevelVarDecl(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveFileScopeVarDeclaration(decl, varMap);
}

static void handleTopLevelFuncDecl(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveFunctionDeclaration(decl, varMap, symbolTable, TOP_LEVEL);
}

/* ---- Block-level declaration handlers ---- */

static void handleBlockLevelVarDecl(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveLocalVarDeclaration(decl, varMap, symbolTable);
}

static void handleBlockLevelFuncDecl(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveFunctionDeclaration(decl, varMap, symbolTable, BLOCK_LEVEL);
}

typedef void (*DeclHandler)(CDeclaration*, SemanticIdentifierMap*, SymbolTable*);

static const DeclHandler topLevelDeclHandlers[] = {
    [DECL_VAR]  = handleTopLevelVarDecl,
    [DECL_FUNC] = handleTopLevelFuncDecl,
};

static const DeclHandler blockLevelDeclHandlers[] = {
    [DECL_VAR]  = handleBlockLevelVarDecl,
    [DECL_FUNC] = handleBlockLevelFuncDecl,
};

/* ---- Level dispatch: chooses which table to use ---- */

static void handleTopLevel(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    DeclHandler handler = topLevelDeclHandlers[decl->type];
    if (!handler) return;
    handler(decl, varMap, symbolTable);
}

static void handleBlockLevel(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    DeclHandler handler = blockLevelDeclHandlers[decl->type];
    if (!handler) return;
    handler(decl, varMap, symbolTable);
}

static const DeclHandler levelHandlers[] = {
    [TOP_LEVEL]   = handleTopLevel,
    [BLOCK_LEVEL] = handleBlockLevel,
};

/* ---- Type check dispatch for resolveDeclarations ---- */

static void typeCheckVarDecl(CDeclaration* decl, SymbolTable* symbolTable) {
    typeCheckFileScopeVariableDeclaration(decl, symbolTable);
}

static void typeCheckFuncDecl(CDeclaration* decl, SymbolTable* symbolTable) {
    typeCheckFunctionDeclaration(decl, symbolTable);
}

typedef void (*TypeCheckHandler)(CDeclaration*, SymbolTable*);

static const TypeCheckHandler declTypeCheckers[] = {
    [DECL_VAR]  = typeCheckVarDecl,
    [DECL_FUNC] = typeCheckFuncDecl,
};

/* ---- Public entry points ---- */

void resolveDeclarations(CDeclarationArray* declarations, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!declarations) return;
    for (int i = 0; i < declarations->size; i++) {
        CDeclaration* declaration = (CDeclaration*)declarations->data[i];
        resolveDeclaration(declaration, varMap, symbolTable, TOP_LEVEL);
        TypeCheckHandler typeChecker = declTypeCheckers[declaration->type];
        if (!typeChecker) continue;
        typeChecker(declaration, symbolTable);
    }
}

void resolveDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable, level declLevel) {
    if (!decl) return;
    DeclHandler handler = levelHandlers[declLevel];
    if (!handler) return;
    handler(decl, varMap, symbolTable);
}
