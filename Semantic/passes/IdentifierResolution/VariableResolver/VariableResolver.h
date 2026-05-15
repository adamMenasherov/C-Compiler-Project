#pragma once
#include "Semantic/semantic.h"

void resolveFileScopeVarDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap);
void resolveLocalVarDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
