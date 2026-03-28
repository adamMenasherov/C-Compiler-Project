#pragma once

char* readSourceFile(char* fileName);
char* generatePreprocessedFileName(char* originalFileName, char **execFileName);
void commandForPreprocessing(char* originalFileName, char* preprocessedFileName);
void commandForExecutable(char* asmFileName, char* executableFileName);
char* generateASMFileName(char* originalFileName);