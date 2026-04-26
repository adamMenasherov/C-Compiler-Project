#pragma once
#include "Parser/AST/C-AST-Nodes/C-ASTNodes.h"
#include "Parser/Parser.h"
#include "DataStructures/Map/Wrappers/SemanticIdentifierMap.h"
#include "DataStructures/HashTable/Wrappers/SymbolTableWrapper.h"

typedef enum {
    TOP_LEVEL,
    BLOCK_LEVEL,
} level;


SymbolTable* resolveAST(AST* ast);
void resolveProgram(CProgram* prog, SymbolTable* symbolTable);
void resolveForInit(CForInit* init, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
void resolveDeclarations(CDeclarationArray* func, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
void resolveFunctionDeclaration(CDeclaration* func, SemanticIdentifierMap* varMap, SymbolTable* symbolTable, level declLevel);
void resolveBlockItem(CBlockItem* blockItem, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
void resolveBlock(CBlock* block, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
void resolveStatement(CStatement* stmt, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
void resolveDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable, level declLevel);
void resolveExpression(CFactor* fact, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
char* generateUniqueVariableName(char* baseName);

// Loop labeling
void resolveBlockWithLabeling(CBlock* block);
void resolveBlockStatementsWithLabeling(CBlockItem* block);
void labelStatement(CStatement* stmt, char* currentLabel);

// Expanded identifier resolution - checking for redeclarations and conflicts in function parameters
void resolveParams(IdentifierArray* params, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);
void resolveFileScopeVarDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap);
void resolveLocalVarDeclaration(CDeclaration* decl, SemanticIdentifierMap* varMap, SymbolTable* symbolTable);

// Type checking
void typeCheckLocalVariableDeclaration(CDeclaration* decl, SymbolTable* symbolTable);
void typeCheckFileScopeVariableDeclaration(CDeclaration* decl, SymbolTable* symbolTable);
void typeCheckFunctionDeclaration(CDeclaration* decl, SymbolTable* symbolTable);
void typeCheckBlock(CBlock* block, SymbolTable* symbolTable);
void typeCheckExpression(CFactor* expr, SymbolTable* symbolTable);
void typeCheckStatement(CStatement* stmt, SymbolTable* symbolTable);
