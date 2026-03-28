#include "Lexer/Tokens/token.h"
#include "Lexer/Tokens/tokenList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "EntryPoint/FileHandling.h"
#include "Lexer/lex.h"
#include "Parser/Parser.h"
#include "ASM-File-Generation/ASMGenerator.h"
#include "Parser/TACKY/TACKY_AST.h"
#include "Parser/TACKY/TACKY_AST_PRINTER.h"


int main(int argc, char* argv[]) {
    char* filename = NULL, *execFileName;
    int stop_at_lex = 0;
    TokenList tokenList;
   
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--lex") == 0) {
            stop_at_lex = 1;
        } 
        else if (argv[i][0] == '-') {} 
        else {
            filename = argv[i]; 
        }
    }

    if (filename == NULL) {
        fprintf(stderr, "Error: No input file specified.\n");
        return 1;
    }

    char* preprocessFileName = generatePreprocessedFileName(filename, &execFileName);
    char* asmFileName = generateASMFileName(filename);
    commandForPreprocessing(filename, preprocessFileName);

    char* source = readSourceFile(preprocessFileName);
    if (!source) {
        fprintf(stderr, "Error: Failed to read preprocessed file %s\n", preprocessFileName);
        free(preprocessFileName);
        return 1;
    }
    
    char* sourcePtr = source; 
    createTokenList(&tokenList);
    lex(&sourcePtr, &tokenList);
    AST* ast = parse(&tokenList);
    TACKY_AST* tacky_ast = astToTACKY_AST(ast);
    printTACKYProgram(tacky_ast->prog);
    /*ASM_AST* asm_ast = astToASM_AST(ast);
    generateASMFile(asm_ast, asmFileName);
    commandForExecutable(asmFileName, execFileName);*/
    
    /*freeASM_AST(asm_ast);*/
    freeAST(ast);
    free(source); 
    free(preprocessFileName);
    freeTokenList(&tokenList);

    return 0;
}

