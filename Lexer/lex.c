#include "lex.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Tokens/token.h"
#include "NFA/nfa.h"
#include "DFA/dfa.h"


void lex(char ** source, TokenList* tokenList) {
    NFA* nfa = createFinalNFA(); // Create the NFA that represents the language of valid tokens.
    DFA* dfa = createDFAFromNFA(nfa); // Convert the NFA to a DFA for efficient token recognition using Subset Construction.
    Token* p; // Pointer to hold the current token being processed.
    while ((p = createToken(source, dfa)) != NULL) {
        /*
            If the token type is ERROR, it means that the lexer encountered an invalid token in the source code.
            The DFA was not able to recognize the token as a valid token type, which indicates a lexical error in the source code.
        */
        if (p->type == ERROR) { 
            fprintf(stderr, "Lexer: Token %s is invalid\n", p->value);
            exit(1);
        }
        addToken(tokenList, p); // Add the recognized token to the token list.
        *source += strlen(p->value); // Move the source pointer forward by the length of the recognized token to continue lexing the next token.
    }

    free(dfa);
    free(nfa);
    printTokenList(tokenList);
    printf("Lexing completed sucessfully.\n");
}