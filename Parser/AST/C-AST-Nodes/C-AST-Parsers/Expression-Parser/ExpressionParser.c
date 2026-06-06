#include "ExpressionParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "../C-ParsersInclude.h"
#include "../../../../Common/SharedTypeRank.h"

#define PARSE_ARRAY_DIMENSIONS(tokens, decl, size)  \
    while (check(tokens, OPEN_BRACKET)) {           \
        expect(tokens, OPEN_BRACKET);               \
        size = getIntegerConstant(tokens);          \
        expect(tokens, CLOSE_BRACKET);              \
        decl = C_CreateArrayDeclarator(decl, size); \
    }

static CDeclarator* parseAbstractDeclarator(TokenList* tokens);
static CDeclarator* parseDirectAbstractDeclarator(TokenList* tokens);
typedef CFactor* (*BinOpHandler)(TokenList* tokens, CFactor* left, int prec);

static CFactor* parsePostfixOperators(CFactor* factor, TokenList* tokens) {
    while (checkIncrementDecrement(tokens)) {
        unaryType type = prefixToPostfix(expectUnaryOp(tokens));
        factor = C_CreateFactorFromUnary(C_CreateUnary(type, factor));
    }

    return factor;
}

static CFactor* handle_assignment(TokenList* tokens, CFactor* left, int prec) {
    expect(tokens, ONE_EQUAL);
    CFactor* right = C_parseExpression(tokens, prec);
    if (!right) return NULL;
    CAssignment* node = C_CreateAssignment(C_CreateCopyOfFactor(left), right);
    return C_CreateFactorFromAssignment(node);
}

static CFactor* handle_compound_assignment(TokenList* tokens, CFactor* left, int prec) {
    binType type = compoundAssignmentToBinType(
        TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array))->type);
    expectCompoundAssignment(tokens);
    CFactor* right = C_parseExpression(tokens, prec);
    if (!right) return NULL;
    CAssignment* node = C_CreateAssignment(
        C_CreateCopyOfFactor(left),
        C_CreateFactorFromBinary(C_CreateBinary(type, C_CreateCopyOfFactor(left), right)));
    return C_CreateFactorFromAssignment(node);
}

static CFactor* handle_ternary(TokenList* tokens, CFactor* left, int prec) {
    expect(tokens, QUESTION_MARK);
    CFactor* middle = C_parseConditionalMiddle(tokens);
    CFactor* right = C_parseExpression(tokens, prec);
    if (!right) return NULL;
    CConditional* node = C_CreateConditional(C_CreateCopyOfFactor(left), middle, right);
    return C_CreateFactorFromConditional(node);
}

static CFactor* handle_binary(TokenList* tokens, CFactor* left, int prec) {
    binType type = expectBinaryOp(tokens);
    CFactor* right = C_parseExpression(tokens, prec + 1);
    if (!right) return NULL;
    CBinary* node = C_CreateBinary(type, C_CreateCopyOfFactor(left), right);
    return C_CreateFactorFromBinary(node);
}

CFactor* C_parseExpression(TokenList* tokens, int min_prec) {
    CFactor* left = C_parseUnaryExpression(tokens);
    if (!left) return NULL;
    int prec;
    while (checkBinaryOp(tokens) && 
        (prec = precedence(TokenArray_get(tokens->array, TokenArray_getCursor(tokens->array)))) >= min_prec) 
    {
        CFactor* new_left;
        if (check(tokens, ONE_EQUAL))      
            new_left = handle_assignment(tokens, left, prec);
        else if (checkCompoundAssignment(tokens)) 
            new_left = handle_compound_assignment(tokens, left, prec);
        else if (check(tokens, QUESTION_MARK))   
            new_left = handle_ternary(tokens, left, prec);
        else
            new_left = handle_binary(tokens, left, prec);
        if (!new_left) 
            return NULL;
        left = new_left;
    }
    return left;
}

CInitializer* C_parseInitializer(TokenList* tokens) {
    if (check(tokens, OPEN_BRACE)) {
        expect(tokens, OPEN_BRACE);
        CInitializerList* initList = CInitializerList_create();
        while (!check(tokens, CLOSE_BRACE)) {
            CInitializerList_append(initList, C_parseInitializer(tokens));
            if (check(tokens, COMMA))
                expect(tokens, COMMA);
        }
        expect(tokens, CLOSE_BRACE);
        return C_CreateCompoundInit(initList);
    }
    CFactor* exp = C_parseExpression(tokens, 0);
    return C_CreateSingleInit(exp);
}


CFactor* C_parseUnaryExpression(TokenList* tokens) {
    if (checkUnaryOp(tokens)) {
        unaryType type = expectUnaryOp(tokens);
        CFactor* exp = C_parseUnaryExpression(tokens);
        if (!exp) return NULL;
        if (type == UNARY_ADDRESS_OF) {
            return C_CreateFactor(FACTOR_ADDRESS_OF, exp);
        }
        if (type == UNARY_DEREFERENCE) {
            return C_CreateFactor(FACTOR_DEREFERENCE, exp);
        }
        return C_CreateFactorFromUnary(C_CreateUnary(type, exp));
    }
    else if (check(tokens, OPEN_PAREN) && lookAheadOneType(tokens)) {
        return parsePostfixOperators(C_CreateFactorFromCast(C_parseCast(tokens)), tokens);
    }
    return C_parsePostfixExpression(tokens);
}

CFactor* C_parsePostfixExpression(TokenList* tokens) {
    CFactor* factor = C_parseFactor(tokens);
    if (!factor) return NULL;
    while (check(tokens, OPEN_BRACKET)) {
        CSubscript* subscript = C_parseSubscript(tokens, factor);
        // The factor becomes the subscript expression
        factor = C_CreateFactorFromSubscript(subscript); 
    }
    return factor;
}


CFactor* C_parseFactor(TokenList* tokens) {
    // Factor is a function call
    if (check(tokens, IDENTIFIER) && lookAheadOne(tokens, OPEN_PAREN)) {
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

    if (checkConstant(tokens)) {
        return parsePostfixOperators(C_CreateFactorFromConstant(C_parseConstant(tokens)), tokens);
    }

    if (check(tokens, IDENTIFIER)) {
        char* identifier = expectIdentifier(tokens);
        return parsePostfixOperators(C_CreateFactorFromVar(C_CreateVar(identifier)), tokens);
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


CFactor* C_parseConditionalMiddle(TokenList* tokens) {
    CFactor* exp = C_parseExpression(tokens, 0);
    expect(tokens, COLON);
    return exp;
}


CConstant* C_parseConstant(TokenList* tokens) {
    int type;
    uint64_t intValue;
    double floatValue;
    expectConstant(tokens, &type, &intValue, &floatValue);
    if (type == CONST_FLOATING_POINT) {
        return C_CreateConstant(intValue, floatValue, type);
    } else {
        return C_CreateConstant(intValue, floatValue, determineConstantType(intValue, type));
    }
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
    CType* type, *declType;
    StorageClass dummySC;
    C_parseTypeAndStorageClass(tokens, &type, &dummySC);
    CDeclarator* decl = parseAbstractDeclarator(tokens);
    expect(tokens, CLOSE_PAREN);
    CFactor* exp = C_parseUnaryExpression(tokens);
    if (!exp) return NULL;
    processDeclarator(decl, type, &declType, NULL, NULL);
    return C_CreateCast(declType, exp);
}

CSubscript* C_parseSubscript(TokenList* tokens, CFactor* array) {
    expect(tokens, OPEN_BRACKET);
    CFactor* index = C_parseExpression(tokens, 0);
    expect(tokens, CLOSE_BRACKET);
    return C_CreateSubscript(array, index);
}

static CDeclarator* parseAbstractDeclarator(TokenList* tokens) {
    if (check(tokens, ASTERISK)) {
        expect(tokens, ASTERISK);
        return C_CreatePointerDeclarator(parseAbstractDeclarator(tokens));
    }
    return parseDirectAbstractDeclarator(tokens);
}

static CDeclarator* parseDirectAbstractDeclarator(TokenList* tokens) {
    int size = 0;
    if (check(tokens, OPEN_PAREN)) {
        expect(tokens, OPEN_PAREN);
        CDeclarator* decl = parseAbstractDeclarator(tokens);
        expect(tokens, CLOSE_PAREN);
        PARSE_ARRAY_DIMENSIONS(tokens, decl, size);
        return decl;
    }
    else if (check(tokens, OPEN_BRACKET)) {
        CDeclarator* decl = NULL;
        PARSE_ARRAY_DIMENSIONS(tokens, decl, size);
        return decl;
    }
    return NULL;
}