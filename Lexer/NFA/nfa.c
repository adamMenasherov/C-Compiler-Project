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
        case CC_UNSIGNED_SUFFIX: return c == 'u' || c == 'U'; break;
        case CC_FLOAT_EXP: return c == 'E' || c == 'e'; break;
        case CC_FLOAT_SIGN: return c == '+' || c == '-'; break;
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

static NFA* nfaForFloat() {
    NFA* nfa = calloc(1, sizeof(NFA));
    if (!nfa) return NULL;

    int q0 = addStateToNFA(nfa, NOT_ACCEPTING);

    // ── Path 1: mantissa? [Ee] [+-]? [0-9]+ ─────────────────────────
    int p1_pre   = addStateToNFA(nfa, NOT_ACCEPTING); // [0-9]* (left mantissa branch)
    int p1_dot_a = addStateToNFA(nfa, NOT_ACCEPTING); // \.
    int p1_post  = addStateToNFA(nfa, NOT_ACCEPTING); // [0-9]+ after dot
    int p1_int   = addStateToNFA(nfa, NOT_ACCEPTING); // [0-9]+ (right mantissa branch)
    int p1_dot_b = addStateToNFA(nfa, NOT_ACCEPTING); // optional \.
    int p1_e     = addStateToNFA(nfa, NOT_ACCEPTING); // [Ee]
    int p1_sign  = addStateToNFA(nfa, NOT_ACCEPTING); // optional [+-]
    int p1_exp   = addStateToNFA(nfa, FLOATING_POINT_CONSTANT); // [0-9]+ after [Ee]

    // mantissa optional: q0 can skip straight to [Ee]
    addEpsiltonTransitionToState(&nfa->states[q0], p1_pre);
    addEpsiltonTransitionToState(&nfa->states[q0], p1_int);
    addEpsiltonTransitionToState(&nfa->states[q0], p1_e);

    // left branch: [0-9]*\.[0-9]+
    addTransitionToNFA(nfa, p1_pre,  p1_pre,   CC_DIGIT, '\0');
    addTransitionToNFA(nfa, p1_pre,  p1_dot_a, CC_EXACT, '.');
    addTransitionToNFA(nfa, p1_dot_a, p1_post, CC_DIGIT, '\0');
    addTransitionToNFA(nfa, p1_post, p1_post,  CC_DIGIT, '\0');
    addEpsiltonTransitionToState(&nfa->states[p1_post], p1_e);

    // right branch: [0-9]+\.?
    addTransitionToNFA(nfa, p1_int, p1_int,   CC_DIGIT, '\0');
    addTransitionToNFA(nfa, p1_int, p1_dot_b, CC_EXACT, '.');
    addEpsiltonTransitionToState(&nfa->states[p1_int],   p1_e); // dot optional
    addEpsiltonTransitionToState(&nfa->states[p1_dot_b], p1_e);

    // exponent: [Ee][+-]?[0-9]+
    addTransitionToNFA(nfa, p1_e,    p1_sign, CC_FLOAT_EXP,  '\0');
    addTransitionToNFA(nfa, p1_sign, p1_exp,  CC_FLOAT_SIGN, '\0');
    addTransitionToNFA(nfa, p1_exp,  p1_exp,  CC_DIGIT,      '\0');
    addEpsiltonTransitionToState(&nfa->states[p1_sign], p1_exp); // sign optional

    // ── Path 2: [0-9]*\.[0-9]+ ───────────────────────────────────────
    int p2_pre  = addStateToNFA(nfa, NOT_ACCEPTING);
    int p2_dot  = addStateToNFA(nfa, NOT_ACCEPTING);
    int p2_post = addStateToNFA(nfa, FLOATING_POINT_CONSTANT);

    addEpsiltonTransitionToState(&nfa->states[q0], p2_pre);
    addTransitionToNFA(nfa, p2_pre,  p2_pre,  CC_DIGIT, '\0');
    addTransitionToNFA(nfa, p2_pre,  p2_dot,  CC_EXACT, '.');
    addTransitionToNFA(nfa, p2_dot,  p2_post, CC_DIGIT, '\0');
    addTransitionToNFA(nfa, p2_post, p2_post, CC_DIGIT, '\0');

    // ── Path 3: [0-9]+\. ─────────────────────────────────────────────
    int p3_digits = addStateToNFA(nfa, NOT_ACCEPTING);
    int p3_dot    = addStateToNFA(nfa, FLOATING_POINT_CONSTANT);

    addEpsiltonTransitionToState(&nfa->states[q0], p3_digits);
    addTransitionToNFA(nfa, p3_digits, p3_digits, CC_DIGIT, '\0');
    addTransitionToNFA(nfa, p3_digits, p3_dot,    CC_EXACT, '.');

    return nfa;
}


static NFA* nfaForConstants() {
    NFA* nfa = calloc(1, sizeof(NFA));
    if (!nfa) return NULL;
    int q0, q1, q2, q3, q4;

    q0 = addStateToNFA(nfa, NOT_ACCEPTING);
    q1 = addStateToNFA(nfa, CONSTANT);
    q2 = addStateToNFA(nfa, LONG_CONSTANT);
    q3 = addStateToNFA(nfa, UNSIGNED_CONSTANT);
    q4 = addStateToNFA(nfa, UNSIGNED_LONG_CONSTANT);

    addTransitionToNFA(nfa, q0, q1, CC_DIGIT, '\0'); 
    addTransitionToNFA(nfa, q1, q1, CC_DIGIT, '\0'); // Loop for digit
    //If reaching the long suffix, then it is a long constant
    addTransitionToNFA(nfa, q1, q2, CC_LONG_SUFFIX, '\0');
    //If reaching the unsigned suffix, then it is an unsigned constant
    addTransitionToNFA(nfa, q1, q3, CC_UNSIGNED_SUFFIX, '\0');
    //If reaching both the unsigned and long suffix, then it is an unsigned long constant
    addTransitionToNFA(nfa, q2, q4, CC_UNSIGNED_SUFFIX, '\0');
    addTransitionToNFA(nfa, q3, q4, CC_LONG_SUFFIX, '\0');

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
    mergeAndConnect(master, nfaForFloat(), masterStart);

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

