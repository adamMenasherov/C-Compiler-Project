#include "C-ASTNodePrinter.h"
#include "../C-ASTOperatorNames.h"
#include <stdio.h>
#include <stdlib.h>

static int depth = 0;

static void indent() {
    for (int i = 0; i < depth; i++) printf("    ");
}

void C_printProgram(CProgram* prog) {
    printf("Program(\n");
    depth++;
    indent(); C_printFunctions(prog->function_def);
    depth--;
    printf("\n)\n");
}

void C_printFunctions(CDeclarationArray* functions) {
    for (int i = 0; i < CDeclarationArray_size(functions); i++) {
        C_printFunction(CDeclarationArray_get(functions, i));
        if (i < CDeclarationArray_size(functions) - 1) printf(",\n");
    }
}

void C_printFunction(CDeclaration* func) {
    if (!func) return;
    if (func->type != DECL_FUNC) {
        fprintf(stderr, "Error: Expected a function declaration/definition\n");
        return;
    }

    printf("Function(\"%s\",\n", func->decl.functionDecl.identifier);
    depth++;

    indent(); printf("Parameters([");
    IdentifierArray* params = func->decl.functionDecl.parameters;
    for (int i = 0; i < params->size; i++) {
        printf("\"%s\"", params->data[i]);
        if (i < params->size - 1) printf(", ");
    }
    printf("]),\n");

    indent(); C_printBlock(func->decl.functionDecl.body);
    depth--;
    printf("\n"); indent(); printf(")");
}

void C_printBlock(CBlock* block) {
    if (!block) return;

    printf("Block([\n");
    depth++;
    DArray_forEach(block->items, elem, {
        CBlockItem* item = (CBlockItem*)elem;
        indent(); C_printBlockItem(item); printf(",\n");
    });
    depth--;
    indent(); printf("])");
}

void C_printReturn(CReturn* returnNode) {
    printf("Return(");
    depth++;
    C_printFactor(returnNode->exp);
    depth--;
    printf(")");
}

void C_printFactor(CFactor* exp) {
    if (!exp) return;

    switch (exp->type) {
        case FACTOR_CONSTANT:    C_printConstant(exp->exp.cnst);             break;
        case FACTOR_UNARY:       C_printUnary(exp->exp.unary);               break;
        case FACTOR_BINARY:      C_printBinary(exp->exp.binary);             break;
        case FACTOR_ASSIGNMENT:  C_printAssignment(exp->exp.assignment);     break;
        case FACTOR_CONDITIONAL: C_printConditional(exp->exp.conditional);   break;
        case FACTOR_VAR:         C_printVar(exp->exp.var);                   break;
        case FACTOR_FUNCTION_CALL: C_printFunctionCall(exp->exp.funcCall);   break;
    }
}

void C_printFunctionCall(CFunctionCall* funcCall) {
    if (!funcCall) return;

    printf("FunctionCall(\"%s\", [", funcCall->identifier);
    ExpressionFactorArray* args = funcCall->arguments;
    for (int i = 0; i < args->size; i++) {
        C_printFactor(args->data[i]);
        if (i < args->size - 1) printf(", ");
    }
    printf("])");
}

void C_printConditional(CConditional* conditional) {
    if (!conditional) return;

    printf("Conditional(\n");
    depth++;
    indent(); C_printFactor(conditional->condition); printf(",\n");
    indent(); C_printFactor(conditional->then);      printf(",\n");
    indent(); C_printFactor(conditional->else_stmt); printf("\n");
    depth--;
    indent(); printf(")");
}

void C_printStatement(CStatement* stmt) {
    switch (stmt->type) {
        case STMT_EXPRESSION: C_printFactor(stmt->stmt.exp); break;
        case STMT_NULL:       C_printNull(); break;
        case STMT_IF:         C_printIf(stmt->stmt.if_stmt); break;
        case STMT_COMPOUND:   C_printBlock(stmt->stmt.compound_stmt->block); break;
        case STMT_RETURN:     C_printReturn(stmt->stmt.ret); break;
        case STMT_BREAK:
            printf("BreakStatement(%s)", stmt->stmt.break_stmt->identifier);
            break;
        case STMT_CONTINUE:
            printf("ContinueStatement(%s)", stmt->stmt.continue_stmt->identifier);
            break;
        case STMT_DO_WHILE: C_printLoop(stmt->type, stmt->stmt.do_while_stmt); break;
        case STMT_WHILE:    C_printLoop(stmt->type, stmt->stmt.while_stmt);    break;
        case STMT_FOR:      C_printForLoop(stmt->stmt.for_stmt);               break;
    }
}

void C_printCompound(CCompound* compound) {
    if (!compound) return;
    printf("Compound(\n");
    depth++;
    indent(); C_printBlock(compound->block);
    depth--;
    printf("\n"); indent(); printf(")");
}

void C_printLoop(statementType loopType, CLoop* loop) {
    if (!loop) return;

    const char* label = loop->identifier ? loop->identifier : "(null)";
    printf("%sLoop(\"%s\",\n", (loopType == STMT_WHILE) ? "While" : "DoWhile", label);
    depth++;
    indent(); C_printFactor(loop->condition); printf(",\n");
    indent(); C_printStatement(loop->body);   printf("\n");
    depth--;
    indent(); printf(")");
}

void C_printForLoop(CForLoop* forLoop) {
    if (!forLoop) return;

    const char* label = forLoop->identifier ? forLoop->identifier : "(null)";
    printf("ForLoop(\"%s\",\n", label);
    depth++;
    indent(); C_printForInit(forLoop->init);         printf(",\n");
    indent(); C_printFactor(forLoop->condition);     printf(",\n");
    indent(); C_printFactor(forLoop->post);          printf(",\n");
    indent(); C_printStatement(forLoop->body);       printf("\n");
    depth--;
    indent(); printf(")");
}

void C_printForInit(CForInit* init) {
    if (!init) return;

    printf("ForInit(");
    switch (init->type) {
        case FOR_INIT_DECL:    C_printVarDeclaration(init->decl); break;
        case FOR_INIT_EXP:     C_printFactor(init->exp);          break;
        case FOR_INIT_WITHOUT: C_printNull();                      break;
    }
    printf(")");
}

void C_printVarDeclaration(CDeclaration* decl) {
    if (!decl) return;
    if (decl->type != DECL_VAR) {
        fprintf(stderr, "Error: Expected a variable declaration\n");
        return;
    }

    printf("Declaration(\"%s\"", decl->decl.variableDecl.identifier);
    if (decl->decl.variableDecl.declType == VAR_DECL_WITH_EXP && decl->decl.variableDecl.exp) {
        printf(", ");
        depth++;
        C_printFactor(decl->decl.variableDecl.exp);
        depth--;
    }
    printf(")");
}

void C_printUnary(CUnary* unary) {
    if (!unary) return;

    printf("Unary(%s, ", getUnaryOpName(unary->type));
    depth++;
    C_printFactor(unary->exp);
    depth--;
    printf(")");
}

void C_printConstant(CConstant* constant) {
    printf("Constant(%d)", constant->val);
}

void C_printBlockItem(CBlockItem* blockItem) {
    switch (blockItem->type) {
        case BLOCK_ITEM_DECL: C_printDeclaration(blockItem->item.decl); break;
        case BLOCK_ITEM_STMT: C_printStatement(blockItem->item.stmt);   break;
    }
}

void C_printDeclaration(CDeclaration* decl) {
    switch (decl->type) {
        case DECL_VAR:  C_printVarDeclaration(decl); break;
        case DECL_FUNC: C_printFunction(decl);        break;
    }
}

void C_printBinary(CBinary* binary) {
    if (!binary) return;

    printf("Binary(%s, ", getBinaryOpName(binary->type));
    depth++;
    C_printFactor(binary->left);
    printf(", ");
    C_printFactor(binary->right);
    depth--;
    printf(")");
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
    depth--;
    printf(")");
}

void C_printNull() {
    printf("NullStatement()");
}

void C_printIf(CIf* if_stmt) {
    if (!if_stmt) return;

    printf("If(%s,\n", (if_stmt->type == IF_WITH_ELSE) ? "WithElse" : "WithoutElse");
    depth++;
    indent(); C_printFactor(if_stmt->condition);  printf(",\n");
    indent(); C_printStatement(if_stmt->then);
    if (if_stmt->else_stmt) {
        printf(",\n");
        indent(); C_printStatement(if_stmt->else_stmt);
    }
    printf("\n");
    depth--;
    indent(); printf(")");
}