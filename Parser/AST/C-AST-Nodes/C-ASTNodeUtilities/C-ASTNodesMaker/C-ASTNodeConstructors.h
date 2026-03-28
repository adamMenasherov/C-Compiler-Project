#pragma once
#include "../C-ASTExpressionTypes.h"
#include "../../C-ASTNodes.h"

CConstant* C_CreateConstant(int val);
CReturn* C_CreateReturn(CExpression* exp);
CFunction* C_CreateFunction(char* function_name, CReturn* body);
CProgram* C_CreateProgram(CFunction* function_def);
CUnary* C_CreateUnary(unaryType type, CExpression* exp);
CExpression* C_CreateExpressionFromConstant(CConstant * exp);
CExpression* C_CreateExpressionFromUnary(CUnary * exp);
CExpression* C_CreateExpression(expType type, void * expVal);