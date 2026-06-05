#pragma once
#include "../Lexer/Tokens/tokenList.h"
#include "AST/C-AST-Nodes/C-ASTNodes.h"
#ifndef PARSER_ONLY
#include "TACKY/TACKYProgram.h"
#include "ASM-Instructions/ASMInstructions.h"
#endif

typedef struct {
    CProgram* prog; 
} AST;

#ifndef PARSER_ONLY
typedef struct {
    TACKYProgram* prog;
} TACKY;

typedef struct {
    ASMProgram* prog;
} ASM;

void printTACKY_AST(TACKY* tacky_ast);
void freeTACKY_AST(TACKY* tacky_ast);
#endif


AST* parse(TokenList* tokens);
#ifndef PARSER_ONLY
TACKY* astToTACKY_AST(AST* ast, SymbolTable* symTable);
ASM* tackyAstToASM_AST(TACKY* ast, SymbolTable* symTable);
void freeASM_AST(ASM* asm_ast);
#endif
void freeAST(AST* ast);
void printAST(AST* ast);
#ifndef PARSER_ONLY
void printASM_AST(ASM* asm_ast);
#endif