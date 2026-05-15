#include "lex.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Tokens/token.h"
#include "NFA/nfa.h"
#include "DFA/dfa.h"


void lex(char ** source, TokenList* tokenList) {
    NFA* nfa = createFinalNFA();
    if (!nfa) {
        fprintf(stderr, "Lexer: failed to create NFA\n");
        exit(1);
    }

    DFA* dfa = createDFAFromNFA(nfa);
    free(nfa);
    if (!dfa) {
        fprintf(stderr, "Lexer: failed to create DFA\n");
        exit(1);
    }

    Token* p;
    while ((p = createToken(source, dfa)) != NULL) {
        if (p->type == ERROR) {
            fprintf(stderr, "Lexer: Token %s is invalid\n", p->value);
            free(p->value);
            free(p);
            free(dfa);
            exit(1);
        }
        addToken(tokenList, p);
        *source += strlen(p->value);
    }

    free(dfa);
    printTokenList(tokenList);
    printf("Lexing completed sucessfully.\n");
}