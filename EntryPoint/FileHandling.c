#include "Lexer/Tokens/token.h"
#include "Lexer/Tokens/tokenList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "EntryPoint/FileHandling.h"
#include "Lexer/lex.h"
#include "Parser/Parser.h"
#include "SemanticAnalysis/semantic.h"
#include "Parser/TACKY/TACKY_AST.h"
#include "Parser/TACKY/TACKYUtils/TACKY_AST_PRINTER.h"
#include "Parser/AST/ASM-AST-Nodes/ASM-ASTNodesUtilities/ASM-ASTNodesPrinter.h"
#include "ASM-File-Generation/ASMGenerator.h"


char* readSourceFile(char* fileName) {
    FILE* fp = fopen(fileName, "r");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp); // Getting the file's size
    rewind(fp);

    char* sourceCode = malloc(sizeof(char) * (fileSize + 1));
    if (!sourceCode) return NULL;

    size_t bytesRead = fread(sourceCode, 1, fileSize, fp); // Reading the source file's contents to the buffer
    sourceCode[bytesRead] = '\0';
    fclose(fp);
    return sourceCode;
}


char* generatePreprocessedFileName(char* originalFileName, char **objectFileName, char **asmFileName) {
    char* fileNameCopy = strdup(originalFileName); // Copying the filename because of strtok
    if (!fileNameCopy) exit(1);

    char* newFileName = malloc(strlen(originalFileName) + 1);
    *objectFileName = malloc(strlen(originalFileName) + 1);
    *asmFileName = malloc(strlen(originalFileName) + 1);
    if (!newFileName || !*objectFileName || !*asmFileName) {
        free(fileNameCopy);
        exit(1);
    }

    // Getting the file's name without the extension
    char* fileNameWithoutExtension = strtok(fileNameCopy, ".");
    if (!fileNameWithoutExtension) {
        free(fileNameCopy);
        free(newFileName);
        free(*objectFileName);
        exit(1);
    }
    

    sprintf(*objectFileName, "%s.o", fileNameWithoutExtension); // Object file's name
    sprintf(newFileName, "%s.i", fileNameWithoutExtension); // Preprocessed file's name
    sprintf(*asmFileName, "%s.s", fileNameWithoutExtension); // ASM file's name
    free(fileNameCopy);
    return newFileName;
}

void commandForPreprocessing(char* originalFileName, char* preprocessedFileName) {
    size_t len = strlen("gcc -E -P ") + strlen(originalFileName) + strlen(" -o ") + strlen(preprocessedFileName) + 1;
    char* command = malloc(len);
    if (!command) exit(1);

    sprintf(command, "gcc -E -P %s -o %s", originalFileName, preprocessedFileName); // Command for preprocessed file
    
    int ret = system(command); // Executing the command 
    if (ret != 0) {
        fprintf(stderr, "Error: Preprocessing failed with exit code %d\n", ret);
    }
    free(command);
}

void commandForObjectFile(char* asmFileName, char* objectFileName) {
    size_t len = strlen("gcc") + strlen(" -c ") + strlen(asmFileName) + strlen(" -o ") + strlen(objectFileName) + 1;
    char* command = malloc(len);
    if (!command) exit(1);
    
    sprintf(command,"gcc -c %s -o %s", asmFileName, objectFileName); // Command for generating object file with -c flag
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Error: generating executable failed with exit code %d\n", ret);
    }
    free(command);
}


char* compileFile(char* fileName) {
    char* objectFileName;
    char* asmFileName;
    TokenList* tokenList = createTokenList();

    // Generating all file names and preprocessing
    char* preprocessFileName = generatePreprocessedFileName(fileName, &objectFileName, &asmFileName);
    commandForPreprocessing(fileName, preprocessFileName); 

    char* source = readSourceFile(preprocessFileName); // Reading source file into buffer
    if (!source) {
        fprintf(stderr, "Error: Failed to read preprocessed file %s\n", preprocessFileName);
        free(preprocessFileName);
        return NULL;
    }
    
    char* sourcePtr = source; 
    lex(&sourcePtr, tokenList); // TokensList is filled with tokens of the source file in the Lexer stage
    AST* ast = parse(tokenList); // Parsing the program to a AST structure
    printAST(ast);
    SymbolTable* symTable = resolveAST(ast);
    printf("\n\n---------------------------------\n\n"); 
    printf("\n\n---------------------------------\n\n");
    symbolTablePrint(symTable);
    printAST(ast);
 
    TACKY_AST* tacky_ast = astToTACKY_AST(ast, symTable);
    printTACKY_AST(tacky_ast);
    ASM_AST* asm_ast = tackyAstToASM_AST(tacky_ast, symTable);
    printASM_AST(asm_ast);
    generateASMFile(asm_ast, asmFileName, symTable);
    commandForObjectFile(asmFileName, objectFileName);
    /*freeASM_AST(asm_ast);
    freeTACKY_AST(tacky_ast);
    freeSymbolTable(symTable);

    freeAST(ast);
    free(source); 
    free(asmFileName);
    free(preprocessFileName);
    freeTokenList(tokenList);*/

    return objectFileName;
}



void startProcess(int argc, char* argv[]) {
    char* finalExecutableName = "a.out";
    int isCFlagPresent = 0, countObject = 0, outputSpecified = 0;
    char** objectFileNames = malloc(sizeof(char*) * argc);
    if (!objectFileNames) {
        fprintf(stderr, "Memory allocation failed for object file names.\n");
        exit(1);
    }

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--lex") == 0 || strcmp(argv[i], "--parse") == 0
                || strcmp(argv[i], "--validate") == 0) continue; // Skipping the flags for now 
        if (strcmp(argv[i], "-c") == 0) {
            isCFlagPresent = 1;
            continue;
        }

        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            finalExecutableName = argv[i + 1];  
            outputSpecified = 1;
            i++; 
            continue;
        }

        else if (strcmp(argv[i], finalExecutableName) == 0) continue;
    
        char* objectFileName = compileFile(argv[i]);
        if (objectFileName == NULL) {
            fprintf(stderr, "Compilation failed for file: %s\n", argv[i]);
            exit(1);
        }
        objectFileNames[countObject++] = objectFileName;
    }

    if (!isCFlagPresent) {
        char* derivedExecutableName = NULL;
        char* executableNameToUse = finalExecutableName;

        if (!outputSpecified && countObject == 1) {
            derivedExecutableName = strdup(objectFileNames[0]);
            if (!derivedExecutableName) {
                fprintf(stderr, "Memory allocation failed for executable name.\n");
                exit(1);
            }

            char* extension = strrchr(derivedExecutableName, '.');
            if (extension) {
                *extension = '\0';
            }
            executableNameToUse = derivedExecutableName;
        }

        generateExecutableCommand(objectFileNames, countObject, executableNameToUse); // Generating executable file name if -c was not present
        free(derivedExecutableName);
    }
    freeObjectFileNames(objectFileNames, countObject);
}


void runExecutableCommand(char* command) {
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Error: Running executable failed with exit code %d\n", ret);
    }
}

void freeObjectFileNames(char** objectFileNames, int count) {
    for (int i = 0; i < count; i++) {
        free(objectFileNames[i]);
    }
    free(objectFileNames);
}

void generateExecutableCommand(char** objectFileNames, int count, char* finalExecutableName) {
    size_t commandLength = strlen("gcc ") + strlen("-o ") + strlen(finalExecutableName) + 1;
    for (int i = 0; i < count; i++) {
        commandLength += strlen(objectFileNames[i]) + 1; // Adding space for each object file name in array
    }

    char* command = malloc(commandLength);
    if (!command) {
        fprintf(stderr, "Memory allocation failed for executable command.\n");
        exit(1);
    }

    // Creating the exec command, adding all of the object files as parameters for the gcc linker. 
    strcpy(command, "gcc ");
    for (int i = 0; i < count; i++) {
        strcat(command, objectFileNames[i]);
        strcat(command, " ");
    }
    strcat(command, "-o ");
    strcat(command, finalExecutableName); // Adding the exec file name. If not given will be 'a.out'

    runExecutableCommand(command); // Running the exec command
    free(command);
}