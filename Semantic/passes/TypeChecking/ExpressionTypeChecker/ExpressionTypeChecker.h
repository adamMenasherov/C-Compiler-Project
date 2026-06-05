#pragma once
#include "Semantic/semantic.h"

void typeCheckExpression(CFactor* expr, SymbolTable* symbolTable);
CFactor* typeCheckAndConvert(CFactor* expr, SymbolTable* symbolTable);
void typeCheckInit(CType* targetType, CInitializer* initializer, SymbolTable* symbolTable);
