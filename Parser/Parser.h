#pragma once
#include "../Lexer/Tokens/tokenList.h"
#include "AST/C-AST-Nodes/C-ASTNodes.h"
#include "TACKY/TACKY_AST.h"
#include "AST/ASM-AST-Nodes/ASM-ASTNodes.h"

typedef struct {
    CProgram* prog; 
} AST;

typedef struct {
    TACKYProgram* prog;
} TACKY_AST;

typedef struct {
    ASMProgram* prog;
} ASM_AST;

void printTACKY_AST(TACKY_AST* tacky_ast);
void freeTACKY_AST(TACKY_AST* tacky_ast);


AST* parse(TokenList* tokens);
TACKY_AST* astToTACKY_AST(AST* ast, SymbolTable* symTable);
ASM_AST* tackyAstToASM_AST(TACKY_AST* ast, SymbolTable* symTable);
void freeASM_AST(ASM_AST* asm_ast);
void freeAST(AST* ast);
void printAST(AST* ast);
void printASM_AST(ASM_AST* asm_ast);