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
            new_exp->exp.unary = C_CreateUnary(((CUnary*)expVal)->type, C_CreateCopyOfFactor(((CUnary*)expVal)->exp));
            break;
        }
            
        case FACTOR_CONSTANT: {
            new_exp->type = FACTOR_CONSTANT;
            new_exp->exp.cnst = C_CreateConstant(((CConstant*)expVal)->val);
            break;
        }
        case FACTOR_BINARY: {
            new_exp->type = FACTOR_BINARY;
            new_exp->exp.binary = C_CreateBinary(((CBinary*)expVal)->type, 
                                C_CreateCopyOfFactor(((CBinary*)expVal)->left), C_CreateCopyOfFactor(((CBinary*)expVal)->right));
            break;
        }       
        case FACTOR_ASSIGNMENT: {
            new_exp->type = FACTOR_ASSIGNMENT;
            new_exp->exp.assignment = C_CreateAssignment(C_CreateCopyOfFactor(((CAssignment*)expVal)->exp1), 
                                    C_CreateCopyOfFactor(((CAssignment*)expVal)->exp2));
            break;
        }
        case FACTOR_VAR: {
            new_exp->type = FACTOR_VAR;
            new_exp->exp.var = C_CreateVar(((CVar*)expVal)->identifier);
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

CFactor* C_CreateFactorFromVar(CVar* var) {
    return C_CreateFactor(FACTOR_VAR, var);
}

CFactor* C_CreateFactorFromBinary(CBinary * exp) {
    return C_CreateFactor(FACTOR_BINARY, exp);
}

CFactor* C_CreateFactorFromAssignment(CAssignment* assign) {
    return C_CreateFactor(FACTOR_ASSIGNMENT, assign);
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
        case STMT_NULL: {
            stmt->stmt.exp = NULL;
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

CVar* C_CreateVar(char* identifier) {
    CVar* var = malloc(sizeof(CVar));
    if (!var) return NULL;
    var->identifier = strdup(identifier);

    return var;
}

CFactor* C_CreateCopyOfFactor(CFactor* original) {
    if (!original) return NULL;
    switch (original->type) {
        case FACTOR_CONSTANT:
            return C_CreateFactorFromConstant(original->exp.cnst);
        case FACTOR_UNARY:
            return C_CreateFactorFromUnary(original->exp.unary);
        case FACTOR_BINARY:
            return C_CreateFactorFromBinary(original->exp.binary);
        case FACTOR_VAR:
            return C_CreateFactorFromVar(original->exp.var);
        case FACTOR_ASSIGNMENT:
            return C_CreateFactorFromAssignment(original->exp.assignment);
        default:
            fprintf(stderr, "Invalid factor type in C_CreateCopyOfFactor\n");
            return NULL;
    }
}

CBinary* C_CreateBinary(binType type, CFactor * left, CFactor * right) {
    CBinary* binary = malloc(sizeof(CBinary));
    if (!binary) return NULL;
    binary->type = type;
    binary->left = left;
    binary->right = right;
    return binary;
}

CDeclaration* C_CreateDecleration(declerationType type, char* iden, CFactor* assign) {
    CDeclaration* decl = malloc(sizeof(CDeclaration));
    if (!decl) return NULL;
    decl->declType = type;
    decl->exp = assign;
    decl->identifier = iden;
    return decl;
}

CAssignment* C_CreateAssignment(CFactor* fact1, CFactor* fact2) {
    CAssignment* assign = malloc(sizeof(CAssignment));
    if (!assign) return NULL;
    assign->exp1 = fact1;
    assign->exp2 = fact2;
    return assign;
}

CBlockItem* C_CreateBlockItem(blockItemType type, void * stmtVal) {
    CBlockItem* blockItem = malloc(sizeof(CBlockItem));
    blockItem->type = type;
    if (!blockItem) return NULL;
    switch(type) {
        case BLOCK_ITEM_DECL: {
            CDeclaration* decl = (CDeclaration*)stmtVal;
            blockItem->item.decl = decl;
            break;
        }
        case BLOCK_ITEM_STMT: {
            CStatement* stmt = (CStatement*)stmtVal;
            blockItem->item.stmt = stmt;
            break;
        }
        default: {
            fprintf(stderr, "Invalid block item type in C_CreateBlockItem\n");
            free(blockItem);
            return NULL;
        }
    }
}