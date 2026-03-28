#pragma once
#include "../../C-ASTNodes.h"

void C_freeProgram(CProgram* prog);
void C_freeFunction(CFunction* func);
void C_freeReturn(CReturn* returnNode);
void C_freeConstant(CConstant* constant);
