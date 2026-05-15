#pragma once
#include "../NFA/nfa.h"
#include "../Tokens/TokenConstsAndPrinter/TokenConsts.h"

typedef struct {
    CharClass cc; // The CharClass of this transition
    int to; // The id of the state the transition leads to
    char exactC; // For CC_EXACT
} DFATransition;

typedef struct {
    int id;
    DFATransition dfaTransitions[MAX_STATES];
    int n_transitions;
    TokenType accepting; 
} DFAState;

typedef struct {
    DFAState states[MAX_STATES];
    int n_states;
} DFA;


/**
 * @brief Creates a DFA from the given NFA using the subset construction algorithm
 * 
 * @param nfa the NFA to convert to a DFA
 * @return DFA* returns the created DFA
 */
DFA* createDFAFromNFA(NFA* nfa);

/**
 * @brief Runs the given keyword through the DFA and returns the TokenType of the accepting state it ends in, or NOT_ACCEPTING if it ends in a non-accepting state 
 * 
 * @param dfa the DFA to run the keyword through
 * @param keyword a pointer to the keyword to run through the DFA; this pointer will be advanced to the end of the recognized token
 * @return TokenType the TokenType of the accepting state the keyword ends in, or NOT_ACCEPTING if it ends in a non-accepting state 
 */
TokenType DFARun(DFA* dfa, char** keyword);  
