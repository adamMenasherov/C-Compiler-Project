#include "C-ASTNodePrinter.h"
#include <stdio.h>
#include <stdlib.h>

void C_printProgram(CProgram* prog) {
    printf("Program(\n");
    C_printFunction(prog->function_def);
    printf("\n)\n");
}

void C_printFunction(CFunction* func) {
    printf("\tFunction(\n");
    printf("\t\tname=\"%s\",\n", func->function_name);
    printf("\t\tbody=");
    C_printReturn(func->body);
    printf("\n\t)");
}

void C_printReturn(CReturn* returnNode) {
    printf("Return(\n");
    C_printExpression(returnNode->exp);
    printf("\n\t\t)");
}

void C_printExpression(CExpression* exp) {
    if (!exp) return;
    
    switch(exp->type) {
        case EXP_CONSTANT:
            printf("\t\t\t");
            C_printConstant(exp->exp.cnst);
            break;
        case EXP_UNARY:
            printf("\t\t\t");
            C_printUnary(exp->exp.unary);
            break;
    }
}

void C_printUnary(CUnary* unary) {
    if (!unary) return;
    
    const char* op_name;
    switch(unary->type) {
        case COMPLEMENT:
            op_name = "Complement";
            break;
        case NEGATE:
            op_name = "Negate";
            break;
        case NOT_UNARY_OP:
            op_name = "NotUnaryOp";
            break;
        default:
            op_name = "Unknown";
    }
    
    printf("Unary(%s, ", op_name);
    C_printExpression(unary->exp);
    printf(")");
}

void C_printConstant(CConstant* constant) {
    printf("Constant(%d)", constant->val);
}
