#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Token* createToken(char** tokenSource, DFA* dfa) {
    if (!tokenSource || !*tokenSource || !dfa) return NULL;
    while (isspace(**tokenSource)) (*tokenSource)++; // Skip leading whitespace
    if (**tokenSource == '\0') return NULL; // No more tokens

    Token* newTok = malloc(sizeof(Token));
    if (!newTok) return NULL; // Allocate memory for the new token

    char* start = *tokenSource;
    char* end = start;
    TokenType type = DFARun(dfa, &end);

    if (type == NOT_ACCEPTING || end == start) {
        end = start + 1; // For printing the invalid token, we take at least one character
        type = ERROR; // Mark as error token
    }

    newTok->type = type;
    newTok->value = strndup(start, (size_t)(end - start));
    if (!newTok->value) {
        free(newTok);
        return NULL;
    }

    return newTok;
}