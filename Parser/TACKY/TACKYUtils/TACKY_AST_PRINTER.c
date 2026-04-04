#include "TACKY_AST_PRINTER.h"
#include "../../AST/C-AST-Nodes/C-ASTNodes.h"
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Helpers                                                             */
/* ------------------------------------------------------------------ */

/** Returns a human-readable string for a unaryType enum value. */
static const char* unaryTypeToString(unaryType type) {
    switch (type) {
        case UNARY_NEGATE:      return "Negate";
        case UNARY_COMPLEMENT:  return "Complement";
        case UNARY_NOT:         return "Not";
        default:                return "Unknown";
    }
}

static const char* binTypeToString(binType type) {
    switch (type) {
        case BIN_ADD:       return "Add";
        case BIN_SUBTRACT:  return "Subtract";
        case BIN_MULTIPLY:  return "Multiply";
        case BIN_DIVIDE:    return "Divide";
        case BIN_REMAINDER:    return "Modulo";
        case BIN_EQUALS:    return "Equals";
        case BIN_NOT_EQUALS:return "NotEquals";
        case BIN_LESS_THAN: return "LessThan";
        case BIN_LESS_EQUAL:return "LessEqual";
        case BIN_GREATER_THAN: return "GreaterThan";
        case BIN_GREATER_EQUAL: return "GreaterEqual";
        default:           return "Unknown";
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
        case TACKY_UNARY: {
            printf("Unary(%s, ", unaryTypeToString(inst->instValue.unaryOp.type));
            printTACKYValue(inst->instValue.unaryOp.src);
            printf(", ");
            printTACKYValue(inst->instValue.unaryOp.dest);
            printf(")");
            break;
        }
        case TACKY_BINARY: {
            printf("Binary(%s, ", binTypeToString(inst->instValue.binaryOp.binaryOpType));
            printTACKYValue(inst->instValue.binaryOp.src1);
            printf(", ");
            printTACKYValue(inst->instValue.binaryOp.src2);
            printf(", ");
            printTACKYValue(inst->instValue.binaryOp.dest);
            printf(")");
            break;
        }
        case TACKY_JUMP:
            printf("Jump(\"%s\")", inst->instValue.jump.label);
            break;
        case TACKY_LABEL:
            printf("Label(\"%s\")", inst->instValue.label.label);
            break;
        case TACKY_JUMP_IF_ZERO:
            printf("JumpIfZero(\"%s\", ", inst->instValue.condJump.label);
            printTACKYValue(inst->instValue.condJump.condition);
            printf(")");
            break;
        case TACKY_JUMP_IF_NOT_ZERO:
            printf("JumpIfNotZero(\"%s\", ", inst->instValue.condJump.label);
            printTACKYValue(inst->instValue.condJump.condition);
            printf(")");
            break;
        case TACKY_COPY:
            printf("Copy(");
            printTACKYValue(inst->instValue.copy.src);
            printf(", ");
            printTACKYValue(inst->instValue.copy.dest);
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

    for (int i = 0; i < InstructionArray_size((InstructionArray*)list); i++) {
        TACKYInstruction* inst = InstructionArray_get((InstructionArray*)list, i);
        if (inst) {
            printTACKYInstruction(inst);
            printf("\n");
        }
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
