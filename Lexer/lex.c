#include "lex.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Tokens/token.h"
#include "NFA/nfa.h"
#include "DFA/dfa.h"


void lex(char ** source, TokenList* tokenList) {
    NFA* nfa = createFinalNFA();
    DFA* dfa = createDFAFromNFA(nfa);
    Token* p;
    while ((p = createToken(source, dfa)) != NULL) {
        if (p->type == ERROR) {
            fprintf(stderr, "Lexer: Token %s is invalid\n", p->value);
            exit(1);
        }
        addToken(tokenList, p);
        *source += strlen(p->value);
    }

    free(dfa);
    free(nfa);
    printTokenList(tokenList);
    printf("Lexing completed sucessfully.\n");
}