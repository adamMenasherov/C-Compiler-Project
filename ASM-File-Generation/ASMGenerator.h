#pragma once
#include "../Parser/Parser.h"
#include "../DataStructures/HashTable/Wrappers/AsmSymbolTableWrapper.h"
#include <stdio.h>

typedef enum {
    REGISTER_8_BIT,
    REGISTER_16_BIT,
    REGISTER_32_BIT,
    REGISTER_64_BIT
} REGISTER_SIZE;


void generateASMFile(ASM* ast, char* asm_file_name, ASMSymbolTable* symbolTable);
void printAsmFileFromAst(ASM *ast, FILE *fp, ASMSymbolTable* symbolTable);
const char * asmUnaryOperatorToString(ASMUnaryOperator op);
const char * asmBinaryOperatorToString(ASMBinaryOperator op);
const char* getRegisterNameForCodeEmission(Register reg, REGISTER_SIZE size);
const char* asmCondCodeToString(ASMCondCode cond);

void printProgramToASMFile(ASMProgram* prog, FILE *fp, ASMSymbolTable* symbolTable);
void printFunctionToASMFile(ASMFunction* func, FILE *fp, ASMSymbolTable* symbolTable);
void printStaticVarToASMFile(ASMStaticVar* staticVar, FILE *fp);
void printStaticConstToASMFile(ASMStaticConst* staticConst, FILE *fp);
void printInstructionsToASMFile(ASMInstruction* inst, FILE *fp, ASMSymbolTable* symbolTable);
void printOperandToASMFile(ASMOperand* op, FILE *fp, REGISTER_SIZE size);
void printFunctionPrologueToASMFile(ASMFunction* func, FILE *fp);
void printFunctionEpilogueToASMFile(FILE* fp);

char* callingWithPLTOrNot(const char* functionName, ASMSymbolTable* symbolTable);
