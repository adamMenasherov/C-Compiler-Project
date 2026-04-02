#pragma once
#include "../Parser/Parser.h"
#include <stdio.h>

void generateASMFile(ASM_AST* ast, char* asm_file_name);
void printAsmFileFromAst(ASM_AST *ast, FILE *fp);

void printProgramToASMFile(ASMProgram* prog, FILE *fp);
void printFunctionToASMFile(ASMFunction* func, FILE *fp);
void printInstructionsToASMFile(ASMInstruction* inst, FILE *fp);
void printOperandToASMFile(ASMOperand* op, FILE *fp);