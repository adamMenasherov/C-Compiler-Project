#include "lex.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Tokens/token.h"


void lex(char ** source, TokenList* tokenList) {
    Token* p;
    while ((p = createToken(source)) != NULL) {
        if (p->type == ERROR) {
            fprintf(stderr, "Lexer: Token %s is invalid\n", p->value);
            freeToken(p);
            exit(1);
        }
        addToken(tokenList, p);
        *source += strlen(p->value);
    }
    printTokenList(tokenList);
    printf("Lexing completed sucessfully.\n");
}