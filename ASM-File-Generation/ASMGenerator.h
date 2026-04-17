#pragma once
#include "../Parser/Parser.h"
#include "../DataStructures/HashTable/Wrappers/SymbolTableWrapper.h"
#include <stdio.h>

typedef enum {
    REGISTER_8_BIT,
    REGISTER_16_BIT,
    REGISTER_32_BIT,
    REGISTER_64_BIT
} REGISTER_SIZE;


void generateASMFile(ASM_AST* ast, char* asm_file_name, SymbolTable* symbolTable);
void printAsmFileFromAst(ASM_AST *ast, FILE *fp, SymbolTable* symbolTable);
const char * asmUnaryOperatorToString(ASMUnaryOperator op);
const char * asmBinaryOperatorToString(ASMBinaryOperator op);
const char* getRegisterNameForCodeEmission(Register reg, REGISTER_SIZE size);
const char* asmCondCodeToString(ASMCondCode cond);

void printProgramToASMFile(ASMProgram* prog, FILE *fp, SymbolTable* symbolTable);
void printFunctionToASMFile(ASMFunction* func, FILE *fp, SymbolTable* symbolTable);
void printInstructionsToASMFile(ASMInstruction* inst, FILE *fp, SymbolTable* symbolTable);
void printOperandToASMFile(ASMOperand* op, FILE *fp, REGISTER_SIZE size);
void printFunctionPrologueToASMFile(ASMFunction* func, FILE *fp);
void printFunctionEpilogueToASMFile(FILE* fp);

char* callingWithPLTOrNot(const char* functionName, SymbolTable* symbolTable);
