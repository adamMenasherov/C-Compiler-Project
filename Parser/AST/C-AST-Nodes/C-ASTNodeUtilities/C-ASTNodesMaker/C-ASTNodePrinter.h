#pragma once
#include "../../C-ASTNodes.h"


void C_printProgram(CProgram* prog);
void C_printFunction(CFunction* func);
void C_printReturn(CReturn* returnNode);
void C_printFactor(CFactor* exp);
void C_printUnary(CUnary* unary);
void C_printConstant(CConstant* constant);
void C_printBinary(CBinary* binary);
void C_printAssignment(CAssignment* assign);
void C_printVar(CVar* var);
void C_printNull(void);
void C_printBlockItem(CBlockItem* blockItem);
void C_printStatement(CStatement* stmt);
void C_printDeclaration(CDeclaration* decl);

