#include "TACKY_AST_PRINTER.h"
#include "../AST/C-AST-Nodes/C-ASTNodes.h"
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Helpers                                                             */
/* ------------------------------------------------------------------ */

/** Returns a human-readable string for a unaryType enum value. */
static const char* unaryTypeToString(unaryType type) {
    switch (type) {
        case UNARY_NEGATE:      return "Negate";
        case UNARY_COMPLEMENT:  return "Complement";
        default:                return "Unknown";
    }
}

/* ------------------------------------------------------------------ */
/*  Public printers                                                     */
/* ------------------------------------------------------------------ */

void printTACKYValue(const TACKYValue* val) {
    if (!val) {
        printf("<null>");
        return;
    }

    switch (val->type) {
        case TACKY_CONSTANT:
            if (val->constant) {
                printf("Constant(%d)", val->constant->value);
            } else {
                printf("Constant(<null>)");
            }
            break;

        case TACKY_VAR:
            if (val->identifier) {
                printf("Var(\"%s\")", val->identifier);
            } else {
                printf("Var(<null>)");
            }
            break;

        default:
            printf("<unknown value type>");
            break;
    }
}

void printTACKYInstruction(const TACKYInstruction* inst) {
    if (!inst) {
        printf("<null instruction>\n");
        return;
    }

    switch (inst->type) {
        case TACKY_UNARY:
            printf("Unary(%s, ", unaryTypeToString(inst->instValue.unaryOp.type));
            printTACKYValue(inst->instValue.unaryOp.src);
            printf(", ");
            printTACKYValue(inst->instValue.unaryOp.dest);
            printf(")");
            break;

        default:
            printf("<unknown instruction type>");
            break;
    }
}

void printTACKYInstructionList(const TACKYInstructionList* list) {
    if (!list) {
        printf("<null instruction list>\n");
        return;
    }

    for (int i = 0; i < list->currSize; i++) {
        printTACKYInstruction(list->instructions[i]);
        printf("\n");
    }
}

void printTACKYReturn(const TACKYReturn* ret) {
    if (!ret) {
        printf("Return(<null>)\n");
        return;
    }

    printf("Return(");
    printTACKYValue(ret->val);
    printf(")\n");
}

void printTACKYFunction(const TACKYFunction* func) {
    if (!func) {
        printf("<null function>\n");
        return;
    }

    printf("-- function: %s --\n",
           func->function_name ? func->function_name : "<unnamed>");

    printTACKYInstructionList(func->instruction_list);
    printTACKYReturn(func->inst);
}

void printTACKYProgram(const TACKYProgram* program) {
    if (!program) {
        printf("<null program>\n");
        return;
    }

    printTACKYFunction(program->function_def);
}
