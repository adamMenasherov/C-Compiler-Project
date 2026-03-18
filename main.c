#include "Tokens/token.h"
#include "Tokens/tokenList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readSourceFile(char* fileName);

int main() {
    char* source= readSourceFile("testFiles/test.c");
    TokenList tokenList;
    Token* p;
    createTokenList(&tokenList);

    while ((p = createToken(&source)) != NULL) {
        if (p->type == ERROR) {
            freeToken(p);
            freeTokenList(&tokenList);
            exit(1);
        }
        addToken(&tokenList, p);
        source += strlen(p->value);
    }
    printTokenList(&tokenList);
    freeTokenList(&tokenList);
    return 0;
}

char* readSourceFile(char* fileName) {
    FILE* fp = fopen(fileName, "r");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    rewind(fp);

    char* sourceCode = malloc(sizeof(char) * fileSize);
    if (!sourceCode) return NULL;

    size_t bytesRead = fread(sourceCode, 1, fileSize, fp);
    sourceCode[bytesRead] = '\0';
    return sourceCode;
}