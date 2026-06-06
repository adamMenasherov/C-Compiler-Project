#pragma once
#include "../Lexer/Tokens/tokenList.h"
#include "AST/C-AST-Nodes/C-ASTNodes.h"
#ifndef PARSER_ONLY
#include "TACKY/TACKYProgram.h"
#ifndef TACKY_ONLY
#include "ASM-Instructions/ASMInstructions.h"
#endif
#endif

typedef struct {
    CProgram* prog; 
} AST;

#ifndef PARSER_ONLY
typedef struct {
    TACKYProgram* prog;
} TACKY;

#ifndef TACKY_ONLY
typedef struct {
    ASMProgram* prog;
} ASM;
#endif

void printTACKY_AST(TACKY* tacky_ast);
void freeTACKY_AST(TACKY* tacky_ast);
#ifndef TACKY_ONLY
void printASM_AST(ASM* asm_ast);
void freeASM_AST(ASM* asm_ast);
#endif
#endif


AST* parse(TokenList* tokens);
#ifndef PARSER_ONLY
TACKY* astToTACKY_AST(AST* ast, SymbolTable* symTable);
#ifndef TACKY_ONLY
ASM* tackyAstToASM_AST(TACKY* ast, SymbolTable* symTable);
#endif
#endif
void freeAST(AST* ast);
void printAST(AST* ast);