#pragma once
#include "Semantic/semantic.h"

void resolveDeclarations(CDeclarationArray* declarations, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
void resolveDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable, level declLevel);
