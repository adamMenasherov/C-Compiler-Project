#pragma once

char* readSourceFile(char* fileName);
char* generatePreprocessedFileName(char* originalFileName, char **objectFileName, char **asmFileName);
void commandForPreprocessing(char* originalFileName, char* preprocessedFileName);
void commandForObjectFile(char* asmFileName, char* executableFileName);
char* compileFile(char* fileName);
void startProcess(int argc, char* argv[]);
void runExecutableCommand(char* command);
void freeObjectFileNames(char** objectFileNames, int count);
void generateExecutableCommand(char** objectFileNames, int count, char* finalExecutableName);