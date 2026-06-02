#pragma once
#include "DataStructures/HashTable/Wrappers/SymbolTableWrapper.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "../TACKY/TACKYProgram.h"

/* Returns storage size in bytes for scalar C types. */
int size(CType* type);
int isSignedType(CType* type);
int isDoubleLabel(const char* identifier);
int isRelationBinaryOp(binType type);
CType* constantTypeToCType(constantType type);
TACKYStaticVarType initialStaticTypeToTACKYStaticVarType(initialValueStaticInitType type);
