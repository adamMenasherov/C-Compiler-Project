#pragma once
#include "Semantic/semantic.h"

void resolveStatement(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
