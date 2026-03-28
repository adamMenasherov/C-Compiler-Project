#pragma once
#include "../ASM-ASTNodes.h"


void printProgram(ASMProgram* prog);
void printFunction(ASMFunction* func);
void printInstructions(Instruction* inst);
void printOperand(Operand* op);