#include "ASM-ASTNodes.h"
#include <stdlib.h>

Operand* parseConstant(CConstant* constant) {
    Operand* op = malloc(sizeof(Operand));
    op->type = IMMEDIATE;
    op->OperandValue.immediate = constant->val;
    return op;
}

Operand* registerOp(Register reg) {
    Operand* op = malloc(sizeof(Operand));
    op->type = REGISTER;
    op->OperandValue.reg = reg;
    return op;
}

Instruction* parseReturn(CReturn* ret) {
    Instruction* inst = malloc(sizeof(Instruction));
    inst->type = MOV;
    inst->operand1 = parseConstant(ret->exp);
    inst->operand2 = registerOp(EAX);
    
    inst->next = malloc(sizeof(Instruction));
    inst->next->type = RET;

    return inst;

}
ASMFunction* parseFunction(CFunction* cFunc) {
    ASMFunction* func = malloc(sizeof(ASMFunction));
    func->function_name = cFunc->function_name;
    func->inst = parseReturn(cFunc->body);

    return func;
}

ASMProgram* parseProgram(CProgram* program) {
    ASMProgram* prog = malloc(sizeof(ASMProgram));
    prog->function_def = parseFunction(program->function_def);

    return prog;
}


char* getRegisterName(Register reg) {
    switch (reg) {
        case EAX: return "eax";
        case RAX: return "rax";
        case RBX: return "rbx";
        case RCX: return "rcx";
        case RDX: return "rdx";
        case RSP: return "rsp";
        case RBP: return "rbp";
        default: return "unknown";
    }
}