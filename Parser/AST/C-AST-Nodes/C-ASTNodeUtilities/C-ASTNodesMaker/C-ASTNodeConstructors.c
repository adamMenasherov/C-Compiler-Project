#include "C-ASTNodeConstructors.h"
#include <stdlib.h>
#include <string.h>



CConstant* C_CreateConstant(int val) {
    CConstant* constant = malloc(sizeof(CConstant));
    if  (!constant) return NULL;

    constant->val = val;
    return constant;
}


CReturn* C_CreateReturn(CExpression* exp) {
    CReturn* returnNode = malloc(sizeof(CReturn));
    if (!returnNode) return NULL;
    returnNode->exp = exp;

    return returnNode;
}


CFunction* C_CreateFunction(char* function_name, CReturn* body) {
    CFunction* func = malloc(sizeof(CFunction));
    if (!func) return NULL;
    func->function_name = strdup(function_name);
    func->body = body;

    return func;
}

CProgram* C_CreateProgram(CFunction* function_def) {
    CProgram* prog = malloc(sizeof(CProgram));
    if (!prog) return NULL;
    prog->function_def = function_def;

    return prog; 
}


CUnary* C_CreateUnary(unaryType type, CExpression* exp) {
    CUnary* unary = malloc(sizeof(CUnary));
    if (!unary) return NULL;
    unary->exp = exp;
    unary->type = type;

    return unary;
}

CExpression* C_CreateExpression(expType type, void * expVal) {
    CExpression* new_exp = calloc(1, sizeof(CExpression));
    if (!new_exp) return NULL;
    switch(type) {
        case EXP_UNARY:
            new_exp->type = EXP_UNARY;
            new_exp->exp.unary = (CUnary*)expVal;
            break;
        case EXP_CONSTANT:
            new_exp->type = EXP_CONSTANT;
            new_exp->exp.cnst = (CConstant*)expVal;
            break;
    }
    return new_exp;
}


CExpression* C_CreateExpressionFromConstant(CConstant * exp) {
    return C_CreateExpression(EXP_CONSTANT, exp);
}

CExpression* C_CreateExpressionFromUnary(CUnary * exp) {
    return C_CreateExpression(EXP_UNARY, exp);
}