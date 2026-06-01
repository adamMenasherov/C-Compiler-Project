#pragma once
#include "../DataStructures/DynamicArray/Wrappers/IdentifierWrapper.h"

char* readSourceFile(char* fileName);
char* generateFileNames(char* originalFileName, char **objectFileName, char **asmFileName);
void commandForPreprocessing(char* originalFileName, char* preprocessedFileName);
void commandForObjectFile(char* asmFileName, char* executableFileName);
char* compileFile(char* fileName);
void startProcess(int argc, char* argv[]);
int runExecutableCommand(char* command);
void freeObjectFileNames(char** objectFileNames, int count);
void generateExecutableCommand(char** objectFileNames, int count, char* finalExecutableName, char* libraryStrings);
int traverseCompilerArgs(int argc, char** argv, char** finalExecutableName, int* outputSpecified, char** objectFileNames,
    int* isCFlagPresent, IdentifierArray* libraries);
char* generateLibraryStrings(IdentifierArray* libraries);