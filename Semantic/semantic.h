#pragma once
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"
#include "Parser/Parser.h"
#include "DataStructures/Map/Wrappers/SemanticIdentifierMap.h"
#include "DataStructures/HashTable/Wrappers/SymbolTableWrapper.h"

typedef enum {
    TOP_LEVEL,
    BLOCK_LEVEL,
} level;

SymbolTable* resolveAST(AST* ast);
void resolveProgram(CProgram* prog, SymbolTable* symbolTable);
