#pragma once
#include "../Parser/Parser.h"
#include <stdio.h>

typedef enum {
    REGISTER_8_BIT,
    REGISTER_16_BIT,
    REGISTER_32_BIT,
    REGISTER_64_BIT
} REGISTER_SIZE;


void generateASMFile(ASM_AST* ast, char* asm_file_name);
void printAsmFileFromAst(ASM_AST *ast, FILE *fp);
const char * asmUnaryOperatorToString(ASMUnaryOperator op);
const char * asmBinaryOperatorToString(ASMBinaryOperator op);
const char* getRegisterNameForCodeEmission(Register reg, REGISTER_SIZE size);
const char* asmCondCodeToString(ASMCondCode cond);

void printProgramToASMFile(ASMProgram* prog, FILE *fp);
void printFunctionToASMFile(ASMFunction* func, FILE *fp);
void printInstructionsToASMFile(ASMInstruction* inst, FILE *fp);
void printOperandToASMFile(ASMOperand* op, FILE *fp, REGISTER_SIZE size);
void printFunctionPrologueToASMFile(ASMFunction* func, FILE *fp);
void printFunctionEpilogueToASMFile(FILE* fp);
