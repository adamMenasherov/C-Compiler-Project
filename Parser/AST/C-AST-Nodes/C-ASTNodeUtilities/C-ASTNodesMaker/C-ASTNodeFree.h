#pragma once
#include "../C-ASTExpressionTypes.h"
#include "../../C-ASTNodes.h"

void C_freeProgram(CProgram* prog);
void C_freeFunctions(CDeclarationArray* functions);
void C_freeFunction(CDeclaration* func);
void C_freeDeclaration(CDeclaration* decl);
void C_freeInitializer(CInitializer* initializer);
void C_freeStatement(CStatement* stmt);
void C_freeBlock(CBlock* block);
void C_freeBlockItem(CBlockItem* item);
void C_freeFactor(CFactor* factor);
void C_freeReturn(CReturn* returnNode);
void C_freeConstant(CConstant* constant);
