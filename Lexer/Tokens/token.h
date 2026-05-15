#pragma once
#include "TokenConstsAndPrinter/TokenConsts.h"
#include "../DFA/dfa.h"

typedef struct {
    TokenType type;
    char* value;
} Token;


Token* createToken(char** tokenSource, DFA* dfa);
