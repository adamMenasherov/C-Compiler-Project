#pragma once
#include "../../semantic.h"

void resolveStatement(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
