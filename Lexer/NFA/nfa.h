#pragma once
#include "../Tokens/TokenConstsAndPrinter/TokenConsts.h"
#define MAX_TRANSITION 16
#define MAX_EPSILON 64
#define MAX_STATES 256


typedef enum {
    CC_ALPHA_UNDER, // [a-zA-Z_]
    CC_ALPHA_DIGIT, // [a-zA-Z0-9_]
    CC_DIGIT, // [0-9]
    CC_FLOAT_EXP, // [Ee]
    CC_FLOAT_SIGN, // [+-] for floating point exponent
    CC_EXACT, // an exact character
    CC_LONG_SUFFIX, // [lL]
    CC_UNSIGNED_SUFFIX, // [uU]
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
    TokenType accepting; // The TokenType of the accepting state, or NOT_ACCEPTING if it's not an accepting state
    int n_transition;
    int n_epsilon;
} NFAState;

/**
 * @brief The NFA struct, which contains an array of NFAState and the number of states in the NFA
 */
typedef struct {
    NFAState states[MAX_STATES]; // The states of the NFA
    int n_states;
} NFA;


/**
 * @brief Create a Final NFA that can recognize all keywords, single character tokens, multiple character operators, identifiers and constants of the C language. 
 * This will be used as the master NFA that we will convert to a DFA and minimize for use in our lexer
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
