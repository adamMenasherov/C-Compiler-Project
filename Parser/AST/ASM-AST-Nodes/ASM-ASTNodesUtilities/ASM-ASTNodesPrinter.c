#include "ASM-ASTNodesPrinter.h"

void printProgram(ASMProgram* prog) {
    printf("Program:\n");
    printFunction(prog->function_def);
}

void printFunction(ASMFunction* func) {
    printf("Function: %s\n", func->function_name);
    printInstructions(func->inst);
}

void printInstructions(Instruction* inst) {
    while (inst) {
        switch (inst->type) {
            case MOV:
                printf("MOV ");
                printOperand(inst->operand1);
                printf(", ");
                printOperand(inst->operand2);
                printf("\n");
                break;
            case RET:
                printf("RET\n");
                break;
        }
        inst = inst->next;
    }
}
void printOperand(Operand* op) {
    if (!op) {
        return;
    }
    switch (op->type) {
        case IMMEDIATE:
            printf("%d", op->OperandValue.immediate);
            break;
        case REGISTER:
            printf("%s", getRegisterName(op->OperandValue.reg));
            break;
    }
}