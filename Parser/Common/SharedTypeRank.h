#pragma once
#include "DataStructures/HashTable/Wrappers/SymbolTableWrapper.h"
#include "Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTExpressionTypes.h"
#include "../TACKY/TACKYProgram.h"

/* Returns storage size in bytes for scalar C specifier types. */
int size(specifierType type);
int isSignedIdentifierType(IdentifierType type);
int isDoubleLabel(const char* identifier);
int isRelationBinaryOp(binType type);
specifierType constantTypeToSpecifierType(constantType type);
TACKYStaticVarType initialStaticTypeToTACKYStaticVarType(initialValueStaticInitType type);