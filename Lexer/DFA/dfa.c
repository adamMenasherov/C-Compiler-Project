#include "dfa.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief 
 * NFASet is used for the subset construction algorithm to track sets of NFA states that correspond to DFA states. 
 * It maintains a sorted list of state ids for efficient comparison and addition.
 */
typedef struct { int ids[MAX_STATES]; int n; } NFASet;

// Checking if a state id is already in the set
static int setHas(const NFASet *s, int id) {
    for (int i = 0; i < s->n; i++) if (s->ids[i] == id) return 1;
    return 0;
}

// Adding a state id to the set while keeping it sorted
static void setAdd(NFASet *s, int id) {
    if (setHas(s, id)) return;
    int i = s->n++;

    // Using insertion sort to maintain sorted order of state ids
    while (i > 0 && s->ids[i-1] > id) { 
        s->ids[i] = s->ids[i-1]; 
        i--; 
    }
    // Insert the new id at the correct position
    s->ids[i] = id;
}

// Checing if two NFA sets are equal by comparing their sizes and sorted state ids (simple linear comparison)
static int setEq(const NFASet *a, const NFASet *b) {
    if (a->n != b->n) return 0;
    for (int i = 0; i < a->n; i++) if (a->ids[i] != b->ids[i]) return 0;
    return 1;
}

static void eClosure(NFA *nfa, NFASet *s) {
    int front = 0;  
    // Traversing all the states in the set
    while (front < s->n) {
        NFAState *st = &nfa->states[s->ids[front++]];
        // Traversing all epsilon transitions from the current state
        for (int j = 0; j < st->n_epsilon; j++) {
            int d = st->epsilon[j].to;
            if (!setHas(s, d)) setAdd(s, d); // Adding the destination state of the epsilon transition to the set if it's not already present
        }
    }
}


// Finding the next NFASet after consuming the input char
static NFASet moveOnChar(NFA *nfa, const NFASet *s, unsigned char input) {
    NFASet r = {0};
    for (int i = 0; i < s->n; i++) {
        NFAState *st = &nfa->states[s->ids[i]];
        for (int j = 0; j < st->n_transition; j++) {
            NFATransition *t = &st->transitions[j];
            if (mapToCharClass((char)input, t->cc, t->exactC))
                setAdd(&r, t->to); // Adding the destination state of the transition to the set if it matches the input character
        }
    }
    return r;
}

// Finding the dominant token type among the NFA states in the set, which will be the accepting token type for the corresponding DFA state
static TokenType dominantToken(NFA *nfa, const NFASet *s) {
    for (int i = 0; i < s->n; i++) {
        TokenType t = nfa->states[s->ids[i]].accepting;
        if (t != NOT_ACCEPTING) return t; // The first accepting token is returned
    }
    return NOT_ACCEPTING;
}


static int findOrCreate(DFA *dfa, NFASet *table, int *nDFA, NFASet *s, NFA *nfa) {
    for (int i = 0; i < *nDFA; i++)
        if (setEq(&table[i], s)) return i;
    if (*nDFA >= MAX_STATES) return -1;
    int idx = (*nDFA)++;
    table[idx] = *s;
    DFAState d = {0}; 
    d.id = idx; 
    d.accepting = dominantToken(nfa, s);
    dfa->states[dfa->n_states++] = d; // Adding the DFAState to the DFA
    return idx;
}


DFA *createDFAFromNFA(NFA *nfa) {
    DFA *dfa = calloc(1, sizeof(DFA));
    if (!dfa) return NULL;

    // Initializing the subset construction algorithm 
    NFASet *table = calloc(MAX_STATES, sizeof(NFASet)); // Table of NFASets corresponding to DFA states
    if (!table) { 
        free(dfa); 
        return NULL; 
    }
    // The initial NFASet for the start state of the DFA is the epsilon closure of the start state of the NFA (which is state 0)
    int nDFA  = 0; // Number of DFA states created so far
    NFASet init = {0};
    setAdd(&init, 0);
    eClosure(nfa, &init); // Calculating the epsilon closure of the initial NFA state
    findOrCreate(dfa, table, &nDFA, &init, nfa);

    for (int di = 0; di < nDFA; di++) {
        DFAState *cur = &dfa->states[di];
        for (int ch = 0; ch <= 255 && cur->n_transitions < MAX_STATES; ch++) {
            NFASet mv = moveOnChar(nfa, &table[di], (unsigned char)ch);
            if (mv.n == 0) continue;
            eClosure(nfa, &mv);
            int dest = findOrCreate(dfa, table, &nDFA, &mv, nfa);
            if (dest < 0) continue; // If we have reached the maximum number of DFA states, we skip creating new transitions
            DFATransition tr = {CC_EXACT, dest, (char)ch}; // Creating a DFA transition for the current character
            cur->dfaTransitions[cur->n_transitions++] = tr; // Adding the transition to the current DFA state
        }
    }

    free(table);
    return dfa;
}

// Returns the next state on char
static int nextStateOnChar(const DFAState *st, unsigned char c) {
    for (int j = 0; j < st->n_transitions; j++) {
        if (st->dfaTransitions[j].cc == CC_EXACT &&
            (unsigned char)st->dfaTransitions[j].exactC == c) {
            return st->dfaTransitions[j].to;
        }
    }
    return -1;
}


TokenType DFARun(DFA *dfa, char **keyword) {
    if (!dfa || !keyword || !*keyword) return NOT_ACCEPTING;

    char *start = *keyword;
    int cur = 0;
    TokenType lastAccepting = dfa->states[cur].accepting;
    char *lastAcceptingPos = (lastAccepting != NOT_ACCEPTING) ? start : NULL;

    for (char *p = start; *p; p++) {
        DFAState *st = &dfa->states[cur];
        int next = nextStateOnChar(st, (unsigned char)*p);
        if (next < 0) break;
        cur = next;
        // If the new state is accepting, we update the last accepting token type and position
        if (dfa->states[cur].accepting != NOT_ACCEPTING) {
            lastAccepting = dfa->states[cur].accepting;
            lastAcceptingPos = p + 1; // The position after the last character of the accepted token
        }
    }
    if (lastAcceptingPos) {
        *keyword = lastAcceptingPos;
        return lastAccepting;
    }

    return NOT_ACCEPTING;
}