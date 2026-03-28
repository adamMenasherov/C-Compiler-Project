#pragma once
#include "../../C-ASTNodes.h"


void C_printProgram(CProgram* prog);
void C_printFunction(CFunction* func);
void C_printReturn(CReturn* returnNode);
void C_printExpression(CExpression* exp);
void C_printUnary(CUnary* unary);
void C_printConstant(CConstant* constant);
