#pragma once
#include "../semantic.h"

char* generateUniqueVariableName(char* baseName);
const char* fromTempToOrigin(char* identifier);
specifierType identifierTypeToSpecifierType(IdentifierType type);
void setTypeVar(CFactor* expr, IdentifierType type);
void setType(CFactor* expr, specifierType type);
void setTypeConst(CFactor* expr, constantType type);
specifierType getCommonType(specifierType type1, specifierType type2);