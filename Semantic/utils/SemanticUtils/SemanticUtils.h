#pragma once
#include "Semantic/semantic.h"
#include "Parser/Common/SharedTypeRank.h"

char* generateUniqueVariableName(char* baseName);
const char* fromTempToOrigin(char* identifier);
specifierType identifierTypeToSpecifierType(IdentifierType type);
void setTypeVar(CFactor* expr, IdentifierType type);
void setType(CFactor* expr, specifierType type);
void setTypeConst(CFactor* expr, constantType type);
specifierType getCommonType(specifierType type1, specifierType type2);
int isBasicType(specifierType type);
IdentifierType specifierTypeToIdentifierType(specifierType type);
initialValueStaticInitType convertExpTypeToStaticInitType(specifierType expType);
specifierType getType(CFactor* factor);
int isSignedSpecifier(specifierType type);
void convertValFromType(uint64_t* val, specifierType toType);