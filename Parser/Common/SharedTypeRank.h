#pragma once

#include "Parser/AST/C-AST-Nodes/C-ASTNodeUtilities/C-ASTExpressionTypes.h"

/* Returns storage size in bytes for scalar C specifier types. */
int size(specifierType type);
