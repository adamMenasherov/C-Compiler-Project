#pragma once
#include "../C-ASTExpressionTypes.h"
#include "../../C-ASTNodes.h"

CConstant* C_CreateConstant(int val);
CReturn* C_CreateReturn(CFactor* exp);
CFunction* C_CreateFunction(char* function_name, CReturn* body);
CProgram* C_CreateProgram(CFunction* function_def);
CUnary* C_CreateUnary(unaryType type, CFactor* exp);
CFactor* C_CreateFactor(factorType type, void * expVal);
CFactor* C_CreateFactorFromConstant(CConstant * exp);
CFactor* C_CreateFactorFromUnary(CUnary * exp);
CFactor* C_CreateFactorFromBinary(CBinary * exp);
CBinary* C_CreateBinary(binType type, CFactor * left, CFactor * right);
