#include "semantic.h"
#include "passes/passes.h"
#include <stdlib.h>

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
