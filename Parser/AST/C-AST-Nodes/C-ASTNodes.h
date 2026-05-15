#pragma once
#include "../../DataStructures/DynamicArray/Wrappers/CDeclarationArrayWrapper.h"
#include "C-AST-Parsers/C-ParserInclude.h"

typedef struct {
    CDeclarationArray* function_def;
} CProgram;

CProgram* C_parseProgram(TokenList* tokens);
CDeclarationArray* C_parseDeclarations(TokenList* tokens);