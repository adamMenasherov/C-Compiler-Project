#pragma once
#include "../Tokens/TokenConstsAndPrinter/TokenConsts.h"
#define MAX_TRANSITION 16
#define MAX_EPSILON 64
#define MAX_STATES 256

typedef enum {
    CC_ALPHA_UNDER, // [a-zA-Z_]
    CC_ALPHA_DIGIT, // [a-zA-Z0-9_]
    CC_DIGIT, // [0-9]
    CC_EXACT, // an exact character
    CC_LONG_SUFFIX, // [lL]
    CC_SIGNED_SUFFIX, // [uU]
    CC_NO_CHARS // For epsilon transitions
} CharClass;

typedef struct {
    CharClass cc; // The CharClass of this transition
    int to; // The id of the state the transition leads to
    char exactC; // For CC_EXACT
} NFATransition;

typedef struct {
    int id; // The state's id
    NFATransition transitions[MAX_TRANSITION]; // The transitions from the current state
    NFATransition epsilon[MAX_EPSILON]; // The epsilon transition from the current state
    TokenType accepting;
    int n_transition;
    int n_epsilon;
} NFAState;

typedef struct {
    NFAState states[MAX_STATES];
    int n_states;
} NFA;


/**
 * @brief Create a Final NFA that can recognize all keywords, single character tokens, multiple character operators, identifiers and constants of the C language. This will be used as the master NFA that we will convert to a DFA and minimize for use in our lexer
 * 
 * @return NFA* returns the created NFA
 */
NFA* createFinalNFA();

/**
 * @brief Maps a character to a specific CharClass and checks if it matches the given CharClass and exact character
 * 
 * @param c The character to be mapped
 * @param cc The CharClass to check against
 * @param exactC The exact character to check against if CharClass is CC_EXACT
 * @return int Returns 1 if the character matches the CharClass and exact character, 0 otherwise
 */
int mapToCharClass(char c, CharClass cc, char exactC);

/**
 * @brief Runs the given keyword through the NFA and returns the TokenType of the accepting state it ends in, or NOT_ACCEPTING if it ends in a non-accepting state
 * 
 * @param nfa the NFA to run the keyword through
 * @param keyword the keyword to run through the NFA
 * @return TokenType the TokenType of the accepting state the keyword ends in, or NOT_ACCEPTING if it ends in a non-accepting state
 */
TokenType NFARun(NFA* nfa, const char* keyword);
