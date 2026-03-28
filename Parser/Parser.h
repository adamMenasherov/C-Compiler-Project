#pragma once
#include "AST/C-AST-Nodes/C-ASTNodes.h"
#include "AST/ASM-AST-Nodes/ASM-ASTNodes.h"
#include "../Lexer/Tokens/tokenList.h"
#include "TACKY/TACKY_AST.h"

typedef struct {
    CProgram* prog; 
} AST;

typedef struct {
    ASMProgram* prog;
} ASM_AST;

typedef struct {
    TACKYProgram* prog;
} TACKY_AST;

AST* parse(TokenList* tokens);
ASM_AST* astToASM_AST(AST* ast);
TACKY_AST* astToTACKY_AST(AST* ast);
void freeASM_AST(ASM_AST* asm_ast);
void freeTACKY_AST(TACKY_AST* tacky_ast);
void freeAST(AST* ast);
void printAST(AST* ast);
void printASM_AST(ASM_AST* asm_ast);
void printTACKY_AST(TACKY_AST* tacky_ast);