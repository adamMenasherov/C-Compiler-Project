#pragma once
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"
#include "Parser/Parser.h"
#include "DataStructures/Map/Wrappers/SemanticIdentifierMap.h"

extern int currGlobalInt;

void resolveAST(AST* ast);
void resolveProgram(CProgram* prog);
void resolveForInit(CForInit* init, SemanticIdentifierMap* varMap);
void resolveFunctions(CDeclarationArray* func);
void resolveFunction(CDeclaration* func);
void labelStatement(CStatement* stmt, char* currentLabel);
void resolveBlockItem(CBlockItem* blockItem, SemanticIdentifierMap* varMap);
void resolveBlockStatementsWithLabeling(CBlockItem* block);
void resolveVarDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap);
void resolveBlock(CBlock* block, SemanticIdentifierMap* varMap);
void resolveBlockWithLabeling(CBlock* block);
void resolveStatement(CStatement* stmt, SemanticIdentifierMap* varMap);
void resolveExpression(CFactor* fact, SemanticIdentifierMap* varMap);
char* generateUniqueVariableName(char* baseName);