#include "TACKYProgram_PRINTER.h"
#include "../../AST/C-AST-Nodes/C-ASTNodes.h"
#include "../../AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTOperatorNames.h"
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Helpers                                                             */
/* ------------------------------------------------------------------ */

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
            printf("Unary(%s, ", getUnaryOpName(inst->instValue.unaryOp.type));
            printTACKYValue(inst->instValue.unaryOp.src);
            printf(", ");
            printTACKYValue(inst->instValue.unaryOp.dest);
            printf(")");
            break;
        }
        case TACKY_BINARY: {
            printf("Binary(%s, ", getBinaryOpName(inst->instValue.binaryOp.binaryOpType));
            printTACKYValue(inst->instValue.binaryOp.src1);
            printf(", ");
            printTACKYValue(inst->instValue.binaryOp.src2);
            printf(", ");
            printTACKYValue(inst->instValue.binaryOp.dest);
            printf(")");
            break;
        }
        case TACKY_TRUNCATE:
            printf("Truncate(");
            printTACKYValue(inst->instValue.truncate.src);
            printf(", ");
            printTACKYValue(inst->instValue.truncate.dest);
            printf(")");
            break;
        case TACKY_SIGN_EXTEND:
            printf("SignExtend(");
            printTACKYValue(inst->instValue.signExtend.src);
            printf(", ");
            printTACKYValue(inst->instValue.signExtend.dest);
            printf(")");
            break;
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
        case TACKY_RETURN:
            printf("Return(");
            printTACKYValue(inst->instValue.returnVal.retVal);
            printf(")");
            break;
        case TACKY_FUNCTION_CALL:
            printf("FunctionCall(\"%s\", [", inst->instValue.funCall.functionName);
            for (int i = 0; i < TACKYValueArray_size(inst->instValue.funCall.args); i++) {
                printTACKYValue(TACKYValueArray_get(inst->instValue.funCall.args, i));
                if (i < TACKYValueArray_size(inst->instValue.funCall.args) - 1) {
                    printf(", ");
                }
            }
            printf("], ");
            printTACKYValue(inst->instValue.funCall.resultVar);
            printf(")");
            break;
        default:
            printf("<unknown instruction type>");
            break;
    }
}

static const char* getStaticInitTypeName(initialValueStaticInitType type) {
    switch (type) {
        case STATIC_INIT_INT:
            return "INT";
        case STATIC_INIT_LONG:
            return "LONG";
        default:
            return "UNKNOWN";
    }
}


void printTACKYStaticVar(const TACKYStaticVar* staticVar) {
    if (!staticVar) {
        printf("<null static var>\n");
        return;
    }

    printf("StaticVar(\"%s\", global=%d, init=%d, static_init_type = %s)\n",
           staticVar->identifier ? staticVar->identifier : "<unnamed>",
           staticVar->global,
            staticVar->initVal.val,
            getStaticInitTypeName(staticVar->initVal.staticInitType));
}

void printTACKYTopLevel(const TACKYTopLevel* topLevel) {
    if (!topLevel) {
        printf("<null top-level>\n");
        return;
    }

    switch (topLevel->type) {
        case TACKY_STATIC_VAR:
            printTACKYStaticVar(topLevel->topLevel.staticVar);
            break;
        case TACKY_FUNC:
            printTACKYFunction(topLevel->topLevel.function);
            break;
        default:
            printf("<unknown top-level type>\n");
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

void printTACKYFunction(const TACKYFunction* func) {
    if (!func) {
        printf("<null function>\n");
        return;
    }

    printf("-- function: %s --\n, global = %d\n",
           func->function_name ? func->function_name : "<unnamed>",
           func->global);

    printTACKYInstructionList(func->instruction_list);
}

void printTACKYProgram(const TACKYProgram* program) {
    if (!program) {
        printf("<null program>\n");
        return;
    }
    printf("Program with %d top-level declaration(s):\n", TACKYTopLevelArray_size(program->topLevels));

    for (int i = 0; i < TACKYTopLevelArray_size(program->topLevels); i++) {
        TACKYTopLevel* topLevel = TACKYTopLevelArray_get(program->topLevels, i);
        printTACKYTopLevel(topLevel);
    }
    
}
