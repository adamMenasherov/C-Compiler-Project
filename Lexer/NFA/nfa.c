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


// NfaForIdentifiers - A simple NFA that accepts identifiers
static NFA* nfaForIdentifiers() {
    NFA* nfa = calloc(1, sizeof(NFA));
    if (!nfa) return NULL;
    int q0, q1;
    
    q0 = addStateToNFA(nfa, NOT_ACCEPTING);
    q1 = addStateToNFA(nfa, IDENTIFIER);

    // Must accept at least a single character to be an identifier, can be underscore or letter
    addTransitionToNFA(nfa, q0, q1, CC_ALPHA_UNDER, '\0'); 
    // remaining characters can be letter or digit
    addTransitionToNFA(nfa, q1, q1, CC_ALPHA_DIGIT, '\0'); 

    return nfa;
}

static NFA* nfaForFloat() {
    NFA* nfa = calloc(1, sizeof(NFA));
    if (!nfa) return NULL;

    int q0 = addStateToNFA(nfa, NOT_ACCEPTING);

    // Path 1: mantissa [Ee] [+-]? [0-9]+  
    int p1_pre = addStateToNFA(nfa, NOT_ACCEPTING); // [0-9]* (left mantissa branch, before the dot)
    int p1_dot_a = addStateToNFA(nfa, NOT_ACCEPTING); // \.
    int p1_post = addStateToNFA(nfa, NOT_ACCEPTING); // [0-9]+ after dot
    int p1_int = addStateToNFA(nfa, NOT_ACCEPTING); // [0-9]+ (right mantissa branch)
    int p1_int_read = addStateToNFA(nfa, NOT_ACCEPTING); // [0-9]+ (right mantissa branch, after reading at least one digit)
    int p1_dot_b = addStateToNFA(nfa, NOT_ACCEPTING); // optional \.
    int p1_e = addStateToNFA(nfa, NOT_ACCEPTING); // [Ee]
    int p1_exp_head = addStateToNFA(nfa, NOT_ACCEPTING); // after [Ee]
    int p1_after_sign = addStateToNFA(nfa, NOT_ACCEPTING); // after optional [+-]
    int p1_exp = addStateToNFA(nfa, FLOATING_POINT_CONSTANT); // [0-9]+, reaching accepting state

    addEpsiltonTransitionToState(&nfa->states[q0], p1_pre); // [0-9]*\.[0-9]+ (digits before dot + digits after dot)
    addEpsiltonTransitionToState(&nfa->states[q0], p1_int); // [0-9]+\.? (digits before dot only, trailing dot optional)

    // left branch: [0-9]*\.[0-9]+ (digits before dot + digits after dot)
    addTransitionToNFA(nfa, p1_pre,  p1_pre,   CC_DIGIT, '\0'); // [0-9]*
    addTransitionToNFA(nfa, p1_pre,  p1_dot_a, CC_EXACT, '.'); // \.
    addTransitionToNFA(nfa, p1_dot_a, p1_post, CC_DIGIT, '\0'); // [0-9]+ after dot
    addTransitionToNFA(nfa, p1_post, p1_post,  CC_DIGIT, '\0'); // [0-9]+ after dot (self loop)
    addEpsiltonTransitionToState(&nfa->states[p1_post], p1_e); // [Ee] after mantissa

    // right branch: [0-9]+\.? (digits before dot only, trailing dot optional, E cannot appear without digits before)
    addTransitionToNFA(nfa, p1_int, p1_int_read,   CC_DIGIT, '\0'); // [0-9]+ (reading at least one digit in the right branch)
    addTransitionToNFA(nfa, p1_int_read, p1_int_read, CC_DIGIT, '\0'); // [0-9]+ (self loop in the right branch)
    addTransitionToNFA(nfa, p1_int_read, p1_dot_b, CC_EXACT, '.'); // optional dot
    addEpsiltonTransitionToState(&nfa->states[p1_int_read], p1_e); // dot optional
    addEpsiltonTransitionToState(&nfa->states[p1_dot_b], p1_e); // dot optional

    // exponent: [Ee][+-]?[0-9]+
    addTransitionToNFA(nfa, p1_e, p1_exp_head, CC_FLOAT_EXP,  '\0'); // [Ee]
    addTransitionToNFA(nfa, p1_exp_head, p1_after_sign, CC_FLOAT_SIGN, '\0'); // optional sign [+-]
    addTransitionToNFA(nfa, p1_exp_head, p1_exp, CC_DIGIT, '\0'); // [0-9]+ (the exponent value), reaching accepting state
    addTransitionToNFA(nfa, p1_after_sign, p1_exp, CC_DIGIT, '\0'); // [0-9]+, reaching accepting state
    addTransitionToNFA(nfa, p1_exp, p1_exp, CC_DIGIT, '\0'); // [0-9]+ (self loop, of the exponent value), reaching accepting state

    // Path 2: [0-9]*\.[0-9]+ - without exponent (mantissa only)
    int p2_pre  = addStateToNFA(nfa, NOT_ACCEPTING);
    int p2_dot  = addStateToNFA(nfa, NOT_ACCEPTING);
    int p2_post = addStateToNFA(nfa, FLOATING_POINT_CONSTANT);

    addEpsiltonTransitionToState(&nfa->states[q0], p2_pre);
    addTransitionToNFA(nfa, p2_pre,  p2_pre,  CC_DIGIT, '\0'); // [0-9]*
    addTransitionToNFA(nfa, p2_pre,  p2_dot,  CC_EXACT, '.'); // \.
    addTransitionToNFA(nfa, p2_dot,  p2_post, CC_DIGIT, '\0'); // [0-9]+ after dot
    addTransitionToNFA(nfa, p2_post, p2_post, CC_DIGIT, '\0'); // [0-9]+ after dot (self loop)

    // Path 3: [0-9]+\. - without exponent (mantissa only, with trailing dot)
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
    // If reaching the long suffix, then it is a long constant
    addTransitionToNFA(nfa, q1, q2, CC_LONG_SUFFIX, '\0');
    // If reaching the unsigned suffix, then it is an unsigned constant
    addTransitionToNFA(nfa, q1, q3, CC_UNSIGNED_SUFFIX, '\0');
    // If reaching both the unsigned and long suffix, then it is an unsigned long constant
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
        // Only accepting if we read the entire keyword
        TokenType accepting = (i == strLen - 1) ? keywordTokenType : NOT_ACCEPTING;
        states[i + 1] = addStateToNFA(nfa, accepting);
        // Each transition is for the exact character in the keyword at that position to the next
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
    addTransitionToNFA(nfa, q0, q1, CC_EXACT, tokenTypeToSymbol[type][0]); // Transition for the single character token

    TokenType compoundType = binaryOpToOpWithEqual(type);
    // In case there's a compound operator for this
    if (compoundType != ERROR) {
        q2 = addStateToNFA(nfa, compoundType); // The state for the compound operator, e.g. +=
        addTransitionToNFA(nfa, q1, q2, CC_EXACT, '='); // Transition for the compound operator, e.g. + to +=

        // In case of ++ and --
        if (type == PLUS || type == HYPHEN) {
            q3 = addStateToNFA(nfa, subAddtoDoubleSubAdd(type));
            addTransitionToNFA(nfa, q1, q3, CC_EXACT, tokenTypeToSymbol[type][0]);
        }
    }
    return nfa;
}

// Handling two char operators
static NFA* nfaForMultipleCharOperators(TokenType type) {
    NFA* nfa = calloc(1, sizeof(NFA));
    if (!nfa) return NULL;
    int q0, q1, q2, q3;
    
    q0 = addStateToNFA(nfa, NOT_ACCEPTING);
    q1 = addStateToNFA(nfa, NOT_ACCEPTING);
    q2 = addStateToNFA(nfa, type);
    // Transition for the first character of the operator to a non-accepting state, e.g. & to & (for &&)
    addTransitionToNFA(nfa, q0, q1, CC_EXACT, tokenTypeToSymbol[type][0]);
    // Transition for the second character of the operator to the accepting state, e.g. & to &&
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
        NFAState newState = sub->states[i]; // Copy the state from the sub NFA
        newState.id = offset + i; // Update the state id to be the next available id in the master NFA

        for (int j = 0; j < newState.n_transition; j++)
            newState.transitions[j].to += offset; // Update the transition target state ids to match the new state ids in the master NFA
        for (int j = 0; j < newState.n_epsilon; j++)
            newState.epsilon[j].to += offset; // Update the epsilon transition target state ids to match the new state ids in the master NFA

        master->states[master->n_states++] = newState; // Add the new state to the master NFA
    }

    return offset; 
}


static void mergeAndConnect(NFA* master, NFA* sub, int masterStart) {
    if (!sub) return;
    int subStart = mergeNFAIntoMaster(master, sub);
    // Adding epsilon transition from the master start state to the sub NFA start state, so that the master NFA can transition to the sub NFA
    addEpsiltonTransitionToState(&master->states[masterStart], subStart); 
    free(sub);
}

NFA* createFinalNFA() {
    NFA* master = calloc(1, sizeof(NFA));
    if (!master) return NULL;
    int masterStart = addStateToNFA(master, NOT_ACCEPTING); // masterStart = 0, first state of the master NFA
    
    // Creating NFA for keywords, and merging
    for (int i = KEYWORD_TOKEN_INT; i < KEYWORD_TOKEN_END_OF_KEYWORDS; i++) {
        TokenType tokenType = keywordTokenTypeToTokenType((KeywordTokenType)i);
        mergeAndConnect(master, nfaForKeywords(tokenTypeToSymbol[tokenType], tokenType), masterStart);
    }
    // Creating NFA for single character tokens, and merging
    for (int i = SINGLE_CHAR_TOKEN_OPEN_PAREN; i < SINGLE_CHAR_END_OF_SINGLE_CHAR_TOKENS; i++) {
        TokenType tokenType = singleCharTokenTypeToTokenType((SingleCharTokenType)i);
        mergeAndConnect(master, nfaForSingleCharTokens(tokenType), masterStart);
    }
    // Creating NFA for multiple character operators, and merging
    for (int i = TWO_CHAR_TOKEN_TWO_AMPERSANDS; i < TWO_CHAR_END_OF_TWO_CHAR_TOKENS; i++) {
        TokenType tokenType = twoCharTokenTypeToTokenType((TwoCharTokenType)i);
        mergeAndConnect(master, nfaForMultipleCharOperators(tokenType), masterStart);
    }

    // Creating NFA for identifiers, constants and floating point constants, and merging
    mergeAndConnect(master, nfaForIdentifiers(), masterStart);
    mergeAndConnect(master, nfaForConstants(), masterStart);
    mergeAndConnect(master, nfaForFloat(), masterStart);

    return master;
}
