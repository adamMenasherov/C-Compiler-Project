#include "ExpressionParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "../C-ParsersInclude.h"

static CFactor* parsePostfixOperators(CFactor* factor, TokenList* tokens) {
    while (checkIncrementDecrement(tokens)) {
        unaryType type = prefixToPostfix(expectUnaryOp(tokens));
        factor = C_CreateFactorFromUnary(C_CreateUnary(type, factor));
    }

    return factor;
}


CFactor* C_parseFactor(TokenList* tokens) {
    if (check(tokens, IDENTIFIER) && lookAheadOne(tokens, OPEN_PAREN)) // Factor is a function call
    {
        char* identifier = expectIdentifier(tokens);
        expect(tokens, OPEN_PAREN);

        CFactor* fact =  parsePostfixOperators(C_CreateFactorFromFunctionCall(
            C_CreateFunctionCall(identifier, parseArgumentList(tokens))), tokens);
        expect(tokens, CLOSE_PAREN);
        return fact;
    }

    if (check(tokens, OPEN_PAREN) && lookAheadOneType(tokens)) {
        return parsePostfixOperators(C_CreateFactorFromCast(C_parseCast(tokens)), tokens);
    }

    if (check(tokens, CONSTANT) || check(tokens, LONG_CONSTANT)) {
        return parsePostfixOperators(C_CreateFactorFromConstant(C_parseConstant(tokens)), tokens);
    }

    if (check(tokens, IDENTIFIER)) {
        char* identifier = expectIdentifier(tokens);
        return parsePostfixOperators(C_CreateFactorFromVar(C_CreateVar(identifier)), tokens);
    }

    else if (checkUnaryOp(tokens)) {
        unaryType type = expectUnaryOp(tokens);
        CFactor* inner_exp = C_parseFactor(tokens);
        return parsePostfixOperators(C_CreateFactorFromUnary(C_CreateUnary(type, inner_exp)), tokens);
    }

    else if (check(tokens, OPEN_PAREN)) {
        expect(tokens, OPEN_PAREN);
        CFactor* inner_exp = C_parseExpression(tokens, 0);
        expect(tokens, CLOSE_PAREN);
        return parsePostfixOperators(inner_exp, tokens);
    }
    else {
        fprintf(stderr, "Expression is invalid\n");
        exit(1);
    } 

    return NULL;
}


CFactor* C_parseExpression(TokenList* tokens, int min_prec) {
    CFactor* left = C_parseFactor(tokens);
    if (!left) return NULL;
    int isBinary = checkBinaryOp(tokens), previous_prec; 
    while (isBinary && (previous_prec = precedence(TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array)))) >= min_prec) {
        if (check(tokens, ONE_EQUAL)) {
            expect(tokens, ONE_EQUAL);
            CFactor* right = C_parseExpression(tokens, previous_prec);
            if (!right) return NULL;
            CAssignment* new_left = C_CreateAssignment(C_CreateCopyOfFactor(left), right);
            left = C_CreateFactorFromAssignment(new_left);
        }
        else if (checkCompoundAssignment(tokens)) {
            binType type = compoundAssignmentToBinType(TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array))->type);
            expectCompoundAssignment(tokens);
            CFactor* right = C_parseExpression(tokens, previous_prec);
            if (!right) return NULL;
            CAssignment* new_left = C_CreateAssignment(C_CreateCopyOfFactor(left), 
                C_CreateFactorFromBinary(C_CreateBinary(type, 
                    C_CreateCopyOfFactor(left), right)));
            left = C_CreateFactorFromAssignment(new_left);
        }
        else if (check(tokens, QUESTION_MARK)) {
            expect(tokens, QUESTION_MARK);
            CFactor* middle = C_parseConditionalMiddle(tokens);
            CFactor* right = C_parseExpression(tokens, previous_prec);
            CConditional* new_left = C_CreateConditional(C_CreateCopyOfFactor(left), middle, right);
            left = C_CreateFactorFromConditional(new_left);
        }
        else {
            binType type = expectBinaryOp(tokens);
            CFactor* right = C_parseExpression(tokens, previous_prec + 1);
            if (!right) return NULL;
            CBinary* new_left = C_CreateBinary(type, C_CreateCopyOfFactor(left), right);
            left = C_CreateFactorFromBinary(new_left);
        }
        
        isBinary = checkBinaryOp(tokens);
    }
    return left;   
}

CFactor* C_parseConditionalMiddle(TokenList* tokens) {
    CFactor* exp = C_parseExpression(tokens, 0);
    expect(tokens, COLON);
    return exp;
}


CConstant* C_parseConstant(TokenList* tokens) {
    uint64_t val = expectConstant(tokens);
    constantType type; 
    if (val > LONG_MAX) {
        fprintf(stderr, "Parser Error: Constant value %lu is too large to fit in a long or int\n", val);
        exit(1);
    }
    if (val <= INT_MAX && !check(tokens, LONG_CONSTANT))
        type = CONST_INT;
    else 
        type = CONST_LONG;

    return C_CreateConstant(val, type);
}

CVar* C_parseVar(TokenList* tokens) {
    char* identifier = expectIdentifier(tokens);
    return C_CreateVar(identifier);
}

ExpressionFactorArray* parseArgumentList(TokenList* tokens) {
    ExpressionFactorArray* arguments = ExpressionFactorArray_create();
    if (!arguments) return NULL;

    if (check(tokens, CLOSE_PAREN)) return arguments;

    while (1) {
        CFactor* arg = C_parseExpression(tokens, 0);
        if (!arg) return arguments;
        ExpressionFactorArray_append(arguments, arg);

        if (!check(tokens, COMMA)) break;
        expect(tokens, COMMA);
    }

    return arguments;
}

CCast* C_parseCast(TokenList* tokens) {
    expect(tokens, OPEN_PAREN);
    specifierType type;
    C_parseTypeAndStorageClass(tokens, &type, &(specifierType){0});
    expect(tokens, CLOSE_PAREN);
    CFactor* exp = C_parseFactor(tokens);
    if (!exp) return NULL;
    return C_CreateCast(type, exp);
}