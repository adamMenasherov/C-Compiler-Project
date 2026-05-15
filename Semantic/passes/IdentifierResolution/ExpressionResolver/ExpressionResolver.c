#include "ExpressionResolver.h"
#include "../../../utils/SemanticUtils/SemanticUtils.h"
#include <stdio.h>
#include <stdlib.h>

static int isIncrementDecrementOpIncludingFix(unaryType type) {
    return type == UNARY_INCREMENT_PREFIX || type == UNARY_INCREMENT_POSTFIX ||
           type == UNARY_DECREMENT_PREFIX || type == UNARY_DECREMENT_POSTFIX;
}

static void handleFactorConstant(CFactor* fact, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    /* nothing to resolve */
}

static void handleFactorUnary(CFactor* fact, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if ((isIncrementDecrementOpIncludingFix(fact->exp.unary->type)) &&
        fact->exp.unary->exp->type != FACTOR_VAR) {
        fprintf(stderr, "Semantic Error: Operand of %s must be a variable\n",
            (fact->exp.unary->type == UNARY_INCREMENT_PREFIX ||
             fact->exp.unary->type == UNARY_INCREMENT_POSTFIX) ? "++" : "--");
        exit(1);
    }
    resolveExpression(fact->exp.unary->exp, varMap, symbolTable);
}

static void handleFactorBinary(CFactor* fact, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveExpression(fact->exp.binary->left, varMap, symbolTable);
    resolveExpression(fact->exp.binary->right, varMap, symbolTable);
}

static void handleFactorVar(CFactor* fact, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    char* uniqueName = semanticMapGet(varMap, fact->exp.var->identifier);
    if (!uniqueName) {
        fprintf(stderr, "Semantic Error: Undeclared variable '%s'\n", fact->exp.var->identifier);
        exit(1);
    }
    fact->exp.var->identifier = uniqueName;
}

static void handleFactorAssignment(CFactor* fact, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (fact->exp.assignment->exp1->type != FACTOR_VAR) {
        fprintf(stderr, "Semantic Error: Left-hand side of assignment must be a variable\n");
        exit(1);
    }
    resolveExpression(fact->exp.assignment->exp1, varMap, symbolTable);
    resolveExpression(fact->exp.assignment->exp2, varMap, symbolTable);
}

static void handleFactorConditional(CFactor* fact, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    resolveExpression(fact->exp.conditional->condition, varMap, symbolTable);
    resolveExpression(fact->exp.conditional->then, varMap, symbolTable);
    resolveExpression(fact->exp.conditional->else_stmt, varMap, symbolTable);
}

static void handleFactorFunctionCall(CFactor* fact, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    char* uniqueName = semanticMapGet(varMap, fact->exp.funcCall->identifier);
    if (!uniqueName) {
        fprintf(stderr, "Semantic Error: Undeclared function '%s'\n", fact->exp.funcCall->identifier);
        exit(1);
    }
    fact->exp.funcCall->identifier = uniqueName;
    ExpressionFactorArray* args = fact->exp.funcCall->arguments;
    if (args) {
        for (int i = 0; i < args->size; i++) {
            CFactor* arg = (CFactor*)args->data[i];
            resolveExpression(arg, varMap, symbolTable);
        }
    }
}

typedef void (*FactorHandler)(CFactor*, SemanticIdentifierMap*, SymbolTable*);

static const FactorHandler factorHandlers[] = {
    [FACTOR_CONSTANT]      = handleFactorConstant,
    [FACTOR_UNARY]         = handleFactorUnary,
    [FACTOR_BINARY]        = handleFactorBinary,
    [FACTOR_VAR]           = handleFactorVar,
    [FACTOR_ASSIGNMENT]    = handleFactorAssignment,
    [FACTOR_CONDITIONAL]   = handleFactorConditional,
    [FACTOR_FUNCTION_CALL] = handleFactorFunctionCall,
};

void resolveExpression(CFactor* fact, SemanticIdentifierMap* varMap, SymbolTable* symbolTable) {
    if (!fact) return;
    FactorHandler handler = factorHandlers[fact->type];
    if (!handler) return;
    handler(fact, varMap, symbolTable);
}
