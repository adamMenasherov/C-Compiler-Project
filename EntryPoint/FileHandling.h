#pragma once

char* readSourceFile(char* fileName);
char* generatePreprocessedFileName(char* originalFileName, char **execFileName);
void commandForPreprocessing(char* originalFileName, char* preprocessedFileName);
void commandForObjectFile(char* asmFileName, char* executableFileName);
char* generateASMFileName(char* originalFileName);
char* compileFile(char* fileName);
void startProcess(int argc, char* argv[]);
void runExecutableCommand(char* command);
void freeObjectFileNames(char** objectFileNames, int count);
void generateExecutableCommand(char** objectFileNames, int count, char* finalExecutableName);