#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C-ASTNodes.h"
#include "C-AST-Parsers/C-ParsersInclude.h"

CProgram* C_parseProgram(TokenList* tokens) {
    CDeclarationArray* functions = C_parseDeclarations(tokens);
    if (!functions) return NULL;

    if (tokensLeft(tokens)) {
        fprintf(stderr, "Parser Error: Invalid tokens left\n");
        exit(1);
    }

    return C_CreateProgram(functions);
}

