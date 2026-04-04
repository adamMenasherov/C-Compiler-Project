#include "Parser.h"
#include <stdlib.h>
#include "AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodeFree.h"
#include "AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodePrinter.h"
#include "TACKY/TACKYUtils/TACKY_AST_FREE.h"
#include "AST/ASM-AST-Nodes/ASM-ASTNodesUtilities/ASM-ASTNodesPrinter.h"
#include <stdio.h>
#include "TACKY/TACKYUtils/TACKY_AST_PRINTER.h"


AST* parse(TokenList* tokens) {
    AST* ast = malloc(sizeof(AST));
    if (!ast) return NULL;
    ast->prog = C_parseProgram(tokens);
    if (!ast->prog) return NULL;
    printf("Parsing completed successfully.\n");

    return ast;
}

ASM_AST* tackyAstToASM_AST(TACKY_AST* ast) {
    ASM_AST* asm_ast = malloc(sizeof(ASM_AST));
    if (!asm_ast) return NULL;
    asm_ast->prog = parseASMprogram(ast->prog);
    if (!asm_ast->prog) {
        free(asm_ast);
        return NULL;
    }
    return asm_ast;
}


TACKY_AST* astToTACKY_AST(AST* ast) {
    TACKY_AST* tacky_ast = malloc(sizeof(TACKY_AST));
    if (!tacky_ast) return NULL;
    tacky_ast->prog = parseTACKYProgram(ast->prog);
    if (!tacky_ast->prog) {
        free(tacky_ast);
        return NULL;
    }
    return tacky_ast;
}


void freeTACKY_AST(TACKY_AST* tacky_ast) {
    if (tacky_ast) {
        freeTackyProgram(tacky_ast->prog);
        free(tacky_ast);
    }
}

void freeASM_AST(ASM_AST* asm_ast) {
    if (asm_ast) {
        free(asm_ast->prog);
        free(asm_ast);
    }
}

void freeAST(AST* ast) {
    if (ast) {
        C_freeProgram(ast->prog);
        free(ast);
    }
}

void printAST(AST* ast) {
    C_printProgram(ast->prog);
}

void printASM_AST(ASM_AST* asm_ast) {
    printASMProgram(asm_ast->prog);
}

void printTACKY_AST(TACKY_AST* tacky_ast) {
    printTACKYProgram(tacky_ast->prog);
}