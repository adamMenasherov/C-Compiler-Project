#pragma once
#include "../../semantic.h"

void resolveFunctionDeclaration(CDeclaration* func, SemanticIdentifierMap* varMap, SymbolTable* symbolTable, level declLevel);
void resolveParams(IdentifierArray* params, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
