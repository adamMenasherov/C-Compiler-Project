#pragma once
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"
#include "Parser/Parser.h"
#include "DataStructures/Map/Wrappers/SemanticIdentifierMap.h"
#include "DataStructures/HashTable/Wrappers/IdentifierTypeInfoWrapper.h"

void resolveAST(AST* ast);
void resolveProgram(CProgram* prog);
void resolveForInit(CForInit* init, SemanticIdentifierMap* varMap);
void resolveFunctions(CDeclarationArray* func);
void resolveFunctionDeclaration(CDeclaration* func, SemanticIdentifierMap* varMap);
void resolveBlockItem(CBlockItem* blockItem, SemanticIdentifierMap* varMap);
void resolveVarDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap);
void resolveBlock(CBlock* block, SemanticIdentifierMap* varMap);
void resolveStatement(CStatement* stmt, SemanticIdentifierMap* varMap);
void resolveExpression(CFactor* fact, SemanticIdentifierMap* varMap);
char* generateUniqueVariableName(char* baseName);

// Loop labeling
void resolveBlockWithLabeling(CBlock* block);
void resolveBlockStatementsWithLabeling(CBlockItem* block);
void labelStatement(CStatement* stmt, char* currentLabel);

// Expanded identifier resolution - checking for redeclarations and conflicts in function parameters
void resolveParams(IdentifierArray* params, SemanticIdentifierMap* varMap);

// Type checking
void typeCheckVariableDeclaration(CDeclaration* decl, IdentifierToTypeTable* identifierTable);
void typeCheckFunctionDeclaration(CDeclaration* decl, IdentifierToTypeTable* identifierTable);
void typeCheckExpression(CFactor* expr, IdentifierToTypeTable* identifierTable);
