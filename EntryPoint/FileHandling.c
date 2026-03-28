#include "FileHandling.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readSourceFile(char* fileName) {
    FILE* fp = fopen(fileName, "r");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    rewind(fp);

    char* sourceCode = malloc(sizeof(char) * (fileSize + 1));
    if (!sourceCode) return NULL;

    size_t bytesRead = fread(sourceCode, 1, fileSize, fp);
    sourceCode[bytesRead] = '\0';
    fclose(fp);
    return sourceCode;
}


char* generatePreprocessedFileName(char* originalFileName, char **execFileName) {
    char* fileNameCopy = strdup(originalFileName), *finalSlash;
    if (!fileNameCopy) exit(1);

    char* newFileName = malloc(strlen(originalFileName) + 1);
    if (!newFileName) {
        free(fileNameCopy);
        exit(1);
    }

    char* fileNameWithoutExtension = strtok(fileNameCopy, ".");
    if (!fileNameWithoutExtension) {
        free(fileNameCopy);
        free(newFileName);
        exit(1);
    }
    *execFileName = strdup(fileNameWithoutExtension);
    if (!*execFileName) {
        free(fileNameCopy);
        free(newFileName);
        exit(1);
    }

    sprintf(newFileName, "%s.i", fileNameWithoutExtension);
    free(fileNameCopy);
    return newFileName;
}


char* generateASMFileName(char* originalFileName) {
    char* fileNameCopy = strdup(originalFileName), *finalSlash;
    if (!fileNameCopy) exit(1);

    char* newFileName = malloc(strlen(originalFileName) + 1);
    if (!newFileName) {
        free(fileNameCopy);
        exit(1);
    }

    char* fileNameWithoutExtension = strtok(fileNameCopy, ".");
    if (!fileNameWithoutExtension) {
        free(fileNameCopy);
        free(newFileName);
        exit(1);
    }

    sprintf(newFileName, "%s.s", fileNameWithoutExtension);
    free(fileNameCopy);
    return newFileName;
}

void commandForPreprocessing(char* originalFileName, char* preprocessedFileName) {
    size_t len = strlen("gcc -E -P ") + strlen(originalFileName) + strlen(" -o ") + strlen(preprocessedFileName) + 1;
    char* command = malloc(len);
    if (!command) exit(1);

    sprintf(command, "gcc -E -P %s -o %s", originalFileName, preprocessedFileName);
    
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Error: Preprocessing failed with exit code %d\n", ret);
    }
    free(command);
}



void commandForExecutable(char* asmFileName, char* executableFileName) {
    size_t len = strlen("gcc") + strlen(asmFileName) + strlen(" -o ") + strlen(executableFileName) + 1;
    char* command = malloc(len);
    if (!command) exit(1);
    
    sprintf(command,"gcc %s -o %s", asmFileName, executableFileName);
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Error: generating executable failed with exit code %d\n", ret);
    }
    free(command);
}