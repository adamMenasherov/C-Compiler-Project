#pragma once
#include "../C-ASTExpressionTypes.h"
#include "../../C-ASTNodes.h"

CConstant* C_CreateConstant(int val, constantType type);
CVar* C_CreateVar(char* identifier);
CReturn* C_CreateReturn(CFactor* exp);
CLoopStmt* C_CreateLoopStmt();
CDeclaration* C_CreateFunction(funcDeclType type, char* identifier, IdentifierArray* parameters, CBlock* body, 
    CFuncType* funcType, specifierType storageClass);

CSwitch* C_CreateSwitch(CFactor* exp);
CCase* C_CreateCase(CConstant* caseConst, CStatement* caseStmt, int hasBreak);
CProgram* C_CreateProgram(CDeclarationArray* function_def);
CUnary* C_CreateUnary(unaryType type, CFactor* exp);
CFactor* C_CreateFactor(factorType type, void * expVal);
CFactor* C_CreateFactorFromConstant(CConstant * exp);
CFactor* C_CreateFactorFromUnary(CUnary * exp);
CFactor* C_CreateFactorFromBinary(CBinary * exp);
CFactor* C_CreateFactorFromVar(CVar* var);
CFactor* C_CreateFactorFromAssignment(CAssignment* assign);
CFactor* C_CreateFactorFromFunctionCall(CFunctionCall* funcCall);
CFactor* C_CreateCopyOfFactor(CFactor* original);
CFactor* C_CreateFactorFromCast(CCast* cast);
CFactor* C_CreateFactorFromConditional(CConditional* conditional);
CBlock* C_CreateBlockFromBlockItems(CBlockItemList* items);
CBlock* C_CreateBlockEmpty();
CLoop* C_CreateLoop(CFactor* condition, CStatement* body);
CForLoop* C_CreateForLoop(CForInit* init, CFactor* condition, CFactor* post, CStatement* body);
CForInit* C_CreateForInit(forInitType type, void* initVal);
CIf* C_CreateIf(ifType type, CFactor* condition, CStatement* then, CStatement* else_stmt);
CBinary* C_CreateBinary(binType type, CFactor * left, CFactor * right);
CStatement* C_CreateStatement(statementType type, void * stmtVal);
CDeclaration* C_CreateVariableDeclaration(varDeclType type, char* iden, CFactor* assign, 
    specifierType varType, specifierType storageClass);
CFuncType* C_CreateType(specifierType type);
CFuncType* C_CreateEmptyType();
CCast* C_CreateCast(specifierType castType, CFactor* exp);
CBlockItem* C_CreateBlockItem(blockItemType type, void * stmtVal);
CAssignment* C_CreateAssignment(CFactor* fact1, CFactor* fact2);
CFunctionCall* C_CreateFunctionCall(char* identifier, ExpressionFactorArray* arguments);
CConditional* C_CreateConditional(CFactor* condition, CFactor* then, CFactor* else_stmt);
CCompound* C_CreateCompound(CBlock* block);
char* generateLoopStmtIdentifier();