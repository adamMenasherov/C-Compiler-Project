#pragma once
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"
#include "Parser/Parser.h"
#include "DataStructures/Map/Wrappers/SemanticVariableMap.h"

extern int currGlobalInt;

void resolveAST(AST* ast);
void resolveProgram(CProgram* prog);
void resolveFunction(CFunction* func);
void resolveBlockItem(CBlockItem* blockItem, SemanticVariableMap* varMap);
void resolveDeclaration(CDeclaration* decl, SemanticVariableMap* varMap);
void resolveBlock(CBlock* block, SemanticVariableMap* varMap);
void resolveStatement(CStatement* stmt, SemanticVariableMap* varMap);
void resolveExpression(CFactor* fact, SemanticVariableMap* varMap);
char* generateUniqueVariableName(char* baseName);