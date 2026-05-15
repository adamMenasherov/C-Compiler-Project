#include "nfa.h"
#include "string.h"
#include <ctype.h>
#include <stdlib.h>


static NFATransition createTransition(int to, CharClass cc, char exactC) {
    NFATransition trans;
    trans.to = to;
    trans.cc = cc;
    trans.exactC = exactC;

    return trans;
}


static void addTransitionToState(NFAState* state, int to, CharClass cc, char exactCC) {
    state->transitions[state->n_transition++] = createTransition(to, cc, exactCC);
}

static void addEpsiltonTransitionToState(NFAState* state, int to) {
    state->epsilon[state->n_epsilon++] = createTransition(to, CC_NO_CHARS, '\0');
}

int mapToCharClass(char c, CharClass cc, char exactC) {
    switch (cc) {
        case CC_ALPHA_UNDER: return isalpha(c) || c == '_'; break;
        case CC_DIGIT: return isdigit(c); break;
        case CC_ALPHA_DIGIT: return isalnum(c) || c == '_'; break;
        case CC_LONG_SUFFIX: return c == 'l' || c == 'L'; break;
        case CC_EXACT: return c == exactC; break;
        default: return 0;
    }
}

static void addTransitionToNFA(NFA* nfa, int from, int to, CharClass cc, char exactC) {
    if (!&nfa->states[to] || !&nfa->states[from]) return;
    addTransitionToState(&nfa->states[from], to, cc, exactC);
}

static int addStateToNFA(NFA* nfa, TokenType accept) {
    NFAState state = {0};
    state.id = nfa->n_states;
    state.accepting = accept;
    state.n_epsilon = state.n_transition = 0;
    nfa->states[nfa->n_states++] = state;

    return state.id;
}


static NFA* nfaForIdentifiers() {
    NFA* nfa = calloc(1, sizeof(NFA));
    if (!nfa) return NULL;
    int q0, q1;
    
    q0 = addStateToNFA(nfa, NOT_ACCEPTING);
    q1 = addStateToNFA(nfa, IDENTIFIER);

    addTransitionToNFA(nfa, q0, q1, CC_ALPHA_UNDER, '\0'); // Must accept at least a single character to be an identifier
    addTransitionToNFA(nfa, q1, q1, CC_ALPHA_DIGIT, '\0'); // Creating inner loop for the accepting state

    return nfa;
}

static NFA* nfaForConstants() {
    NFA* nfa = calloc(1, sizeof(NFA));
    if (!nfa) return NULL;
    int q0, q1, q2;

    q0 = addStateToNFA(nfa, NOT_ACCEPTING);
    q1 = addStateToNFA(nfa, CONSTANT);
    q2 = addStateToNFA(nfa, LONG_CONSTANT);

    addTransitionToNFA(nfa, q0, q1, CC_DIGIT, '\0'); 
    addTransitionToNFA(nfa, q1, q1, CC_DIGIT, '\0'); // Loop for digit
    addTransitionToNFA(nfa, q1, q2, CC_LONG_SUFFIX, '\0'); // If reaching the long suffix, then it is a long constant
    addTransitionToNFA(nfa, q2, q2, CC_LONG_SUFFIX, '\0'); // Allow multiple long suffixes, because they're valid

    return nfa;
}


static NFA* nfaForKeywords(const char* keyword, TokenType keywordTokenType) {
    if (!keyword) return NULL;
    NFA* nfa = calloc(1, sizeof(NFA));
    if (!nfa) return NULL;
    int states[MAX_STATES];
    int strLen = strlen(keyword);

    states[0] = addStateToNFA(nfa, NOT_ACCEPTING);
    for (int i = 0; i < strLen; i++) {
        TokenType accepting = (i == strLen - 1) ? keywordTokenType : NOT_ACCEPTING;
        states[i + 1] = addStateToNFA(nfa, accepting);
        addTransitionToNFA(nfa, states[i], states[i + 1], CC_EXACT, keyword[i]);
    }
    return nfa;
}

static NFA* nfaForSingleCharTokens(TokenType type) {
    NFA* nfa = calloc(1, sizeof(NFA));
    if (!nfa) return NULL;
    int q0, q1, q2, q3;
    
    q0 = addStateToNFA(nfa, NOT_ACCEPTING);
    q1 = addStateToNFA(nfa, type);
    addTransitionToNFA(nfa, q0, q1, CC_EXACT, tokenTypeToSymbol[type][0]);

    TokenType compoundType = binaryOpToOpWithEqual(type);
    if (compoundType != ERROR) {
        q2 = addStateToNFA(nfa, compoundType);
        addTransitionToNFA(nfa, q1, q2, CC_EXACT, '=');

        if (type == PLUS || type == HYPHEN) {
            q3 = addStateToNFA(nfa, subAddtoDoubleSubAdd(type));
            addTransitionToNFA(nfa, q1, q3, CC_EXACT, tokenTypeToSymbol[type][0]);
        }
    }
    return nfa;
}

static NFA* nfaForMultipleCharOperators(TokenType type) {
    NFA* nfa = calloc(1, sizeof(NFA));
    if (!nfa) return NULL;
    int q0, q1, q2, q3;
    
    q0 = addStateToNFA(nfa, NOT_ACCEPTING);
    q1 = addStateToNFA(nfa, NOT_ACCEPTING);
    q2 = addStateToNFA(nfa, type);
    addTransitionToNFA(nfa, q0, q1, CC_EXACT, tokenTypeToSymbol[type][0]);
    addTransitionToNFA(nfa, q1, q2, CC_EXACT, tokenTypeToSymbol[type][0]);

    TokenType compoundType = binaryOpToOpWithEqual(type);
    if (compoundType != ERROR) {
        q3 = addStateToNFA(nfa, compoundType);
        addTransitionToNFA(nfa, q2, q3, CC_EXACT, '=');
    }
    return nfa;
}

static int mergeNFAIntoMaster(NFA* master, NFA* sub) {
    int offset = master->n_states;

    for (int i = 0; i < sub->n_states; i++) {
        NFAState newState = sub->states[i];
        newState.id = offset + i;

        for (int j = 0; j < newState.n_transition; j++)
            newState.transitions[j].to += offset;
        for (int j = 0; j < newState.n_epsilon; j++)
            newState.epsilon[j].to += offset;

        master->states[master->n_states++] = newState;
    }

    return offset; 
}


static void mergeAndConnect(NFA* master, NFA* sub, int masterStart) {
    if (!sub) return;
    int subStart = mergeNFAIntoMaster(master, sub);
    addEpsiltonTransitionToState(&master->states[masterStart], subStart);
    free(sub);
}

NFA* createFinalNFA() {
    NFA* master = calloc(1, sizeof(NFA));
    if (!master) return NULL;
    int masterStart = addStateToNFA(master, NOT_ACCEPTING); // masterStart = 0, first state of the master NFA
    
    for (int i = KEYWORD_TOKEN_INT; i < KEYWORD_TOKEN_END_OF_KEYWORDS; i++) {
        TokenType tokenType = keywordTokenTypeToTokenType((KeywordTokenType)i);
        mergeAndConnect(master, nfaForKeywords(tokenTypeToSymbol[tokenType], tokenType), masterStart);
    }
    for (int i = SINGLE_CHAR_TOKEN_OPEN_PAREN; i < SINGLE_CHAR_END_OF_SINGLE_CHAR_TOKENS; i++) {
        TokenType tokenType = singleCharTokenTypeToTokenType((SingleCharTokenType)i);
        mergeAndConnect(master, nfaForSingleCharTokens(tokenType), masterStart);
    }
    for (int i = TWO_CHAR_TOKEN_TWO_AMPERSANDS; i < TWO_CHAR_END_OF_TWO_CHAR_TOKENS; i++) {
        TokenType tokenType = twoCharTokenTypeToTokenType((TwoCharTokenType)i);
        mergeAndConnect(master, nfaForMultipleCharOperators(tokenType), masterStart);
    }

    mergeAndConnect(master, nfaForIdentifiers(), masterStart);
    mergeAndConnect(master, nfaForConstants(), masterStart);

    return master;
}

TokenType NFARun(NFA* nfa, const char* keyword) {
    NFAState master = nfa->states[0];
    TokenType accepting = NOT_ACCEPTING;
    for (int j = 0; j < master.n_epsilon; j++) {
        int i = 0, keywordIdx = 0; 
        NFAState current = nfa->states[master.epsilon[j].to];
        if (current.accepting != NOT_ACCEPTING) accepting = current.accepting;

        while (keyword[keywordIdx] && i < current.n_transition) {
            if (mapToCharClass(keyword[keywordIdx], current.transitions[i].cc, current.transitions[i].exactC)) {
                NFAState next = nfa->states[current.transitions[i].to];
                // Accepting keyword only if we read all of it and reached an accepting state
                if (next.accepting != NOT_ACCEPTING && keyword[keywordIdx + 1] == '\0') {
                    accepting = next.accepting;
                    return accepting;
                }
                current = next;
                keywordIdx++;
                i = 0;
            } else {
                i++;
            }
        }
    }
    
    return accepting;
}

