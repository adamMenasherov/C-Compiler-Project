#include "C-ASTNodePrinter.h"
#include "../C-ASTOperatorNames.h"
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
    C_printBlock(func->block);
    printf("\n\t)");
}

void C_printBlock(CBlock* block) {
    if (!block) return;

    printf("Block([\n");
    DArray_forEach(block->items, elem,
    {
        CBlockItem* item = (CBlockItem*)elem;
        for (int i = 0; i < depth; i++) printf("\t");
        C_printBlockItem(item);
        printf(",\n");
    });
    
    printf("])");
}

void C_printReturn(CReturn* returnNode) {
    printf("Return(");
    depth = 3;
    C_printFactor(returnNode->exp);
    printf(")");
}

void C_printFactor(CFactor* exp) {
    if (!exp) return;

    switch (exp->type) {
        case FACTOR_CONSTANT: C_printConstant(exp->exp.cnst);  break;
        case FACTOR_UNARY:    C_printUnary(exp->exp.unary);    break;
        case FACTOR_BINARY:   C_printBinary(exp->exp.binary);  break;
        case FACTOR_ASSIGNMENT: C_printAssignment(exp->exp.assignment); break;
        case FACTOR_CONDITIONAL: C_printConditional(exp->exp.conditional); break;
        case FACTOR_VAR: C_printVar(exp->exp.var); break;
    }
}

void C_printConditional(CConditional* conditional) {
    if (!conditional) return;

    printf("Conditional(");
    depth++;
    C_printFactor(conditional->condition);
    printf(",");
    C_printFactor(conditional->then);
    printf(",");
    C_printFactor(conditional->else_stmt);
    printf(")");
    depth--;
}

void C_printStatement(CStatement* stmt) {
    switch(stmt->type) {
        case STMT_EXPRESSION: C_printFactor(stmt->stmt.exp); break;
        case STMT_NULL: C_printNull(); break;
        case STMT_IF: C_printIf(stmt->stmt.if_stmt); break;
        case STMT_COMPOUND: C_printBlock(stmt->stmt.compound_stmt->block); break;
        case STMT_RETURN: C_printReturn(stmt->stmt.ret); break;
    }
}

void C_printCompound(CCompound* compound) {
    if (!compound) return;
    printf("Compound(\n\t");
    C_printBlock(compound->block);
    printf("\n)");
}

void C_printDeclaration(CDeclaration* decl) {
    if (!decl) return;
    
    printf("Declaration(\"%s\"", decl->identifier);
    
    if (decl->declType == DECL_WITH_EXP && decl->exp) {
        printf(", ");
        depth++;
        C_printFactor(decl->exp);
        printf(")");
        depth--;
    } else {
        printf(")");
    }
}

void C_printUnary(CUnary* unary) {
    if (!unary) return;

    printf("Unary(%s,", getUnaryOpName(unary->type));
    depth++;
    C_printFactor(unary->exp);
    printf(")");
    depth--;
}

void C_printConstant(CConstant* constant) {
    printf("Constant(%d)", constant->val);
}

void C_printBlockItem(CBlockItem* blockItem) {
    switch (blockItem->type) {
        case BLOCK_ITEM_DECL: C_printDeclaration(blockItem->item.decl); break;
        case BLOCK_ITEM_STMT: C_printStatement(blockItem->item.stmt); break;
    }
}

void C_printBinary(CBinary* binary) {
    if (!binary) return;

    printf("Binary(%s, ", getBinaryOpName(binary->type));
    depth++;
    C_printFactor(binary->left);
    printf(", ");
    C_printFactor(binary->right);
    printf(")");
    depth--;
}

void C_printVar(CVar* var) {
    if (!var) return;
    printf("Var(\"%s\")", var->identifier);
}

void C_printAssignment(CAssignment* assign) {
    if (!assign) return;
    
    printf("Assignment(");
    depth++;
    C_printFactor(assign->exp1);
    printf(", ");
    C_printFactor(assign->exp2);
    printf(")");
    depth--;
}

void C_printNull() {
    printf("NullStatement()");
}

void C_printIf(CIf* if_stmt) {
    if (!if_stmt) return;

    printf("If(%s, ", (if_stmt->type == IF_WITH_ELSE) ? "WithElse" : "WithoutElse");
    depth++;
    C_printFactor(if_stmt->condition);
    printf(", ");
    C_printStatement(if_stmt->then);
    if (if_stmt->else_stmt) {
        printf(", ");
        C_printStatement(if_stmt->else_stmt);
    }
    printf(")");
    depth--;
}
