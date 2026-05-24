#pragma once
#include "../Lexer/Tokens/tokenList.h"
#include "AST/C-AST-Nodes/C-ASTNodes.h"
#include "TACKY/TACKYProgram.h"
#include "ASM-Instructions/ASMInstructions.h"

typedef struct {
    CProgram* prog; 
} AST;

typedef struct {
    TACKYProgram* prog;
} TACKY;

typedef struct {
    ASMProgram* prog;
} ASM;

void printTACKY_AST(TACKY* tacky_ast);
void freeTACKY_AST(TACKY* tacky_ast);


AST* parse(TokenList* tokens);
TACKY* astToTACKY_AST(AST* ast, SymbolTable* symTable);
ASM* tackyAstToASM_AST(TACKY* ast, SymbolTable* symTable);
void freeASM_AST(ASM* asm_ast);
void freeAST(AST* ast);
void printAST(AST* ast);
void printASM_AST(ASM* asm_ast);