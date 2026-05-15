#pragma once
#include "Semantic/semantic.h"

void resolveBlock(CBlock* block, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
void resolveBlockItem(CBlockItem* blockItem, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
