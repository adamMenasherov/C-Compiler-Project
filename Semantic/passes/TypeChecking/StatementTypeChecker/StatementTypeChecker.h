#pragma once
#include "Semantic/semantic.h"

void typeCheckStatement(CStatement* stmt, SymbolTable* symbolTable);
void setCurrentFunctionReturnType(specifierType retType);
