#pragma once
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"
#include "Parser/Parser.h"
#include "DataStructures/Map/Wrappers/SemanticVariableMap.h"

extern int currGlobalInt;

void resolveAST(AST* ast);
void resolveProgram(CProgram* prog);
void resolveForInit(CForInit* init, SemanticVariableMap* varMap);
void resolveFunction(CFunction* func);
void labelStatement(CStatement* stmt, char* currentLabel);
void resolveBlockItem(CBlockItem* blockItem, SemanticVariableMap* varMap);
void resolveBlockStatementsWithLabeling(CBlockItem* block);
void resolveDeclaration(CDeclaration* decl, SemanticVariableMap* varMap);
void resolveBlock(CBlock* block, SemanticVariableMap* varMap);
void resolveBlockWithLabeling(CBlock* block);
void resolveStatement(CStatement* stmt, SemanticVariableMap* varMap);
void resolveExpression(CFactor* fact, SemanticVariableMap* varMap);
char* generateUniqueVariableName(char* baseName);