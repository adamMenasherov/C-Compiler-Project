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
    C_printFactor(returnNode->exp);
    printf("\n\t\t)");
}

void C_printFactor(CFactor* exp) {
    if (!exp) return;

    switch (exp->type) {
        case FACTOR_CONSTANT: C_printConstant(exp->exp.cnst);  break;
        case FACTOR_UNARY:    C_printUnary(exp->exp.unary);    break;
        case FACTOR_BINARY:   C_printBinary(exp->exp.binary);  break;
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
    C_printFactor(unary->exp);
    printf(")");
    depth--;
}

void C_printConstant(CConstant* constant) {
    printf("Constant(%d)", constant->val);
}

void C_printBinary(CBinary* binary) {
    if (!binary) return;

    const char* op_name;
    switch (binary->type) {
        case BIN_ADD:       op_name = "Add";      break;
        case BIN_SUBTRACT:  op_name = "Subtract"; break;
        case BIN_MULTIPLY:  op_name = "Multiply"; break;
        case BIN_DIVIDE:    op_name = "Divide";   break;
        case BIN_MODULO:    op_name = "Modulo";   break;
        default:           op_name = "Unknown";
    }

    printf("Binary(%s,\n", op_name);
    depth++;
    for (int i = 0; i < depth; i++) printf("\t");
    C_printFactor(binary->left);
    printf(",\n");
    for (int i = 0; i < depth; i++) printf("\t");
    C_printFactor(binary->right);
    printf(")");
    depth--;
}


