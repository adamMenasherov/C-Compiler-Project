#pragma once
#include "Semantic/semantic.h"

void resolveFunctionDeclaration(CDeclaration* func, SemanticIdentifierMap* varMap, SymbolTable* symbolTable, level declLevel);
void resolveParams(IdentifierArray* params, CFuncType* funcType, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
