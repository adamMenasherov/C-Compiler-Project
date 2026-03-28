#pragma once
#include "../C-AST-Nodes/C-ASTNodes.h"

// Forward declarations
typedef struct Instruction Instruction;
typedef struct ASMFunction ASMFunction;

typedef enum {
    MOV,
    RET
} InstructionType;

typedef enum {
    REGISTER,
    IMMEDIATE
} OperandType;

typedef enum {
    EAX,
    RAX,
    RBX,
    RCX,
    RDX,
    RSP,
    RBP
} Register;

typedef struct {
    OperandType type;
    union {
        Register reg;
        int immediate;
    } OperandValue;
} Operand;


typedef struct {
    ASMFunction * function_def;
} ASMProgram;

typedef struct Instruction {
    InstructionType type;
    Operand* operand1;
    Operand* operand2;
    Instruction* next;
} Instruction;

typedef struct ASMFunction {
    char* function_name; 
    Instruction* inst; 
} ASMFunction;


Operand* parseConstant(CConstant* constant);
Operand* registerOp(Register reg);
Instruction* parseReturn(CReturn* ret);
ASMFunction* parseFunction(CFunction* func);
ASMProgram* parseProgram(CProgram* program);
char* getRegisterName(Register reg);