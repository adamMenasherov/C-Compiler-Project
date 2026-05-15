#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Token* createToken(char** tokenSource, DFA* dfa) {
    if (!tokenSource || !*tokenSource || !dfa) return NULL;

    while (isspace((unsigned char)**tokenSource)) (*tokenSource)++;
    if (**tokenSource == '\0') return NULL; // No more tokens

    Token* newTok = malloc(sizeof(Token));
    if (!newTok) return NULL;

    char* start = *tokenSource;
    char* end = start;
    TokenType type = DFARun(dfa, &end);

    if (type == NOT_ACCEPTING || end == start) {
        end = start + 1;
        type = ERROR;
    }

    newTok->type = type;
    newTok->value = strndup(start, (size_t)(end - start));
    if (!newTok->value) {
        free(newTok);
        return NULL;
    }

    return newTok;
}