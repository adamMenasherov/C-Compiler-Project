#pragma once
#include "../../C-ASTNodes.h"


void C_printProgram(CProgram* prog);
void C_printFunction(CDeclaration* func);
void C_printReturn(CReturn* returnNode);
void C_printFactor(CFactor* exp);
void C_printUnary(CUnary* unary);
void C_printForInit(CForInit* init);  
void C_printFunctionCall(CFunctionCall* funcCall);
void C_printLoop(statementType loopType, CLoop* loop);
void C_printForLoop(CForLoop* forLoop);
void C_printSwitch(CSwitch* switch_stmt);
void C_printCase(CCase* case_stmt);
void C_printCast(CCast* cast);
void C_printConstant(CConstant* constant);
void C_printBinary(CBinary* binary);
void C_printAssignment(CAssignment* assign);
void C_printDereference(CFactor* exp);
void C_printAddressOf(CFactor* exp);
void C_printConditional(CConditional* conditional);
void C_printSubscript(CSubscript* subscript);
void C_printDeclaration(CDeclaration* decl);
void C_printInitializer(CInitializer* init);
void C_printDeclarations(CDeclarationArray* declArr);
void C_printVar(CVar* var);
void C_printIf(CIf* if_stmt);
void C_printNull(void);
void C_printBlockItem(CBlockItem* blockItem);
void C_printStatement(CStatement* stmt);
void C_printVarDeclaration(CDeclaration* decl);
void C_printCompound(CCompound* compound);
void C_printBlock(CBlock* block);
char* getCTypeName(CType* type, char* buf, size_t size);
char* C_getDeclaratorIdentifier(CDeclarator* decl);

