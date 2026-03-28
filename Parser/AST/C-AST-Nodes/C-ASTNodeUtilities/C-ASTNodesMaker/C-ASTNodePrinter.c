#include "C-ASTNodePrinter.h"
#include <stdio.h>
#include <stdlib.h>

static int depth = 0;

void C_printProgram(CProgram* prog) {
    printf("Program(\n\t");
    C_printFunction(prog->function_def);
    printf("\n)\n");
}

void C_printFunction(CFunction* func) {
    printf("Function(\"%s\",\n\t\t", func->function_name);
    C_printReturn(func->body);
    printf("\n\t)");
}

void C_printReturn(CReturn* returnNode) {
    printf("Return(\n\t\t\t");
    depth = 3;
    C_printExpression(returnNode->exp);
    printf("\n\t\t)");
}

void C_printExpression(CExpression* exp) {
    if (!exp) return;

    switch (exp->type) {
        case EXP_CONSTANT: C_printConstant(exp->exp.cnst);  break;
        case EXP_UNARY:    C_printUnary(exp->exp.unary);    break;
    }
}

void C_printUnary(CUnary* unary) {
    if (!unary) return;

    const char* op_name;
    switch (unary->type) {
        case UNARY_COMPLEMENT:   op_name = "Complement"; break;
        case UNARY_NEGATE:       op_name = "Negate";     break;
        case UNARY_NOT_UNARY_OP: op_name = "Not";        break;
        default:           op_name = "Unknown";
    }

    printf("Unary(%s,\n", op_name);
    depth++;
    for (int i = 0; i < depth; i++) printf("\t");
    C_printExpression(unary->exp);
    printf(")");
    depth--;
}

void C_printConstant(CConstant* constant) {
    printf("Constant(%d)", constant->val);
}