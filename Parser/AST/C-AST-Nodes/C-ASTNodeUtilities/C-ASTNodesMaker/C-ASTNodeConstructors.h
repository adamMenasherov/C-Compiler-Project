#pragma once
#include "../C-ASTExpressionTypes.h"
#include "../../C-ASTNodes.h"

CConstant* C_CreateConstant(int val);
CVar* C_CreateVar(char* identifier);
CReturn* C_CreateReturn(CFactor* exp);
CFunction* C_CreateFunction(char* function_name, CBlock* block);
CProgram* C_CreateProgram(CFunction* function_def);
CUnary* C_CreateUnary(unaryType type, CFactor* exp);
CFactor* C_CreateFactor(factorType type, void * expVal);
CFactor* C_CreateFactorFromConstant(CConstant * exp);
CFactor* C_CreateFactorFromUnary(CUnary * exp);
CFactor* C_CreateFactorFromBinary(CBinary * exp);
CFactor* C_CreateFactorFromVar(CVar* var);
CFactor* C_CreateFactorFromAssignment(CAssignment* assign);
CFactor* C_CreateCopyOfFactor(CFactor* original);
CFactor* C_CreateFactorFromConditional(CConditional* conditional);
CBlock* C_CreateBlockFromBlockItems(CBlockItemList* items);
CBlock* C_CreateBlockEmpty();
CIf* C_CreateIf(ifType type, CFactor* condition, CStatement* then, CStatement* else_stmt);
CBinary* C_CreateBinary(binType type, CFactor * left, CFactor * right);
CStatement* C_CreateStatement(statementType type, void * stmtVal);
CDeclaration* C_CreateDecleration(declerationType type, char* iden, CFactor* assign);
CBlockItem* C_CreateBlockItem(blockItemType type, void * stmtVal);
CAssignment* C_CreateAssignment(CFactor* fact1, CFactor* fact2);
CConditional* C_CreateConditional(CFactor* condition, CFactor* then, CFactor* else_stmt);
CCompound* C_CreateCompound(CBlock* block);
