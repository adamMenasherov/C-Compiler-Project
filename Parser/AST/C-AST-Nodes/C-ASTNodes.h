#pragma once
#include "DataStructures/DynamicArray/Wrappers/CDeclarationWrapper.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "Lexer/Tokens/tokenList.h"

typedef struct {
    CDeclarationArray* function_def;
} CProgram;

CProgram* C_parseProgram(TokenList* tokens);
CDeclarationArray* C_parseDeclarations(TokenList* tokens);