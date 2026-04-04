#include "C-ASTNodeConstructors.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


CConstant* C_CreateConstant(int val) {
    CConstant* constant = malloc(sizeof(CConstant));
    if  (!constant) return NULL;

    constant->val = val;
    return constant;
}


CReturn* C_CreateReturn(CFactor* exp) {
    CReturn* returnNode = malloc(sizeof(CReturn));
    if (!returnNode) return NULL;
    returnNode->exp = exp;

    return returnNode;
}


CFunction* C_CreateFunction(char* function_name, CBlockItemList* body) {
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


CUnary* C_CreateUnary(unaryType type, CFactor* exp) {
    CUnary* unary = malloc(sizeof(CUnary));
    if (!unary) return NULL;
    unary->exp = exp;
    unary->type = type;

    return unary;
}

CFactor* C_CreateFactor(factorType type, void * expVal) {
    CFactor* new_exp = calloc(1, sizeof(CFactor));
    if (!new_exp) return NULL;
    switch(type) {
        case FACTOR_UNARY: {
            new_exp->type = FACTOR_UNARY;
            new_exp->exp.unary = (CUnary*)expVal;
            break;
        }
            
        case FACTOR_CONSTANT: {
            new_exp->type = FACTOR_CONSTANT;
            new_exp->exp.cnst = (CConstant*)expVal;
            break;
        }
        case FACTOR_BINARY: {
            new_exp->type = FACTOR_BINARY;
            new_exp->exp.binary = (CBinary*)expVal;
            break;
        }       
    }
    return new_exp;
}


CFactor* C_CreateFactorFromConstant(CConstant * exp) {
    return C_CreateFactor(FACTOR_CONSTANT, exp);
}

CFactor* C_CreateFactorFromUnary(CUnary * exp) {
    return C_CreateFactor(FACTOR_UNARY, exp);
}

CFactor* C_CreateFactorFromBinary(CBinary * exp) {
    return C_CreateFactor(FACTOR_BINARY, exp);
}

CStatement* C_CreateStatement(statementType type, void * stmtVal) {
    CStatement* stmt = malloc(sizeof(CStatement));
    if (!stmt) return NULL;
    stmt->type = type;
    switch(type) {
        case STMT_RETURN: {
            stmt->stmt.ret = (CReturn*)stmtVal;
            break;
        }
        case STMT_EXPRESSION: {
            stmt->stmt.exp = (CFactor*)stmtVal;
            break;
        }
        default: {
            fprintf(stderr, "Invalid statement type in C_CreateStatement\n");
            free(stmt);
            return NULL;
        }
    }
    return stmt;
}




CBinary* C_CreateBinary(binType type, CFactor * left, CFactor * right) {
    CBinary* binary = malloc(sizeof(CBinary));
    if (!binary) return NULL;
    binary->type = type;
    binary->left = left;
    binary->right = right;
    return binary;
}