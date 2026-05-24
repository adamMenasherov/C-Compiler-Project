#include "Parser.h"
#include <stdlib.h>
#include "AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodeFree.h"
#include "AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTNodesMaker/C-ASTNodePrinter.h"
#include <stdio.h>
#include "ASM-Instructions/ASMInstructionsUtilities/ASMInstructionsFree.h"
#include "TACKY/TACKYUtils/TACKYProgram_FREE.h"
#include "TACKY/TACKYUtils/TACKYProgram_PRINTER.h"
#include "ASM-Instructions/ASMInstructionsUtilities/ASMInstructionsPrinter.h"


AST* parse(TokenList* tokens) {
    AST* ast = malloc(sizeof(AST));
    if (!ast) return NULL;
    ast->prog = C_parseProgram(tokens);
    if (!ast->prog) return NULL;
    printf("Parsing completed successfully.\n");

    return ast;
}

TACKY* astToTACKY_AST(AST* ast, SymbolTable* symTable) {
    TACKY* tacky_ast = malloc(sizeof(TACKY));
    if (!tacky_ast) return NULL;
    tacky_ast->prog = parseTACKYProgram(ast->prog, symTable);
    if (!tacky_ast->prog) {
        free(tacky_ast);
        return NULL;
    }
    return tacky_ast;
}

void freeTACKY_AST(TACKY* tacky_ast) {
    if (tacky_ast) {
        freeTackyProgram(tacky_ast->prog);
        free(tacky_ast);
    }
}

void printTACKY_AST(TACKY* tacky_ast) {
    if (tacky_ast) {
        printTACKYProgram(tacky_ast->prog);
    }
}


ASM* tackyAstToASM_AST(TACKY* ast, SymbolTable* symTable) {
    ASM* asm_ast = malloc(sizeof(ASM));
    if (!asm_ast) return NULL;
    asm_ast->prog = parseASMprogram(ast->prog, symTable);
    if (!asm_ast->prog) {
        free(asm_ast);
        return NULL;
    }
    return asm_ast;
}

void freeASM_AST(ASM* asm_ast) {
    if (asm_ast) {
        freeASMProgram(asm_ast->prog);
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

void printASM_AST(ASM* asm_ast) {
    printASMProgram(asm_ast->prog);
}