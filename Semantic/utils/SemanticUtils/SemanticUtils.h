#pragma once
#include "Semantic/semantic.h"
#include "Parser/Common/SharedTypeRank.h"

char* generateUniqueVariableName(char* baseName);
const char* fromTempToOrigin(char* identifier);
void setTypeVar(CFactor* expr, CType* type);
void setType(CFactor* expr, CType* type);
void setTypeConst(CFactor* expr, constantType type);
CType* getCommonType(CType* type1, CType* type2);
int isBasicType(CType* type);
initialValueStaticInitType convertExpTypeToStaticInitType(CType* expType);
CType* getType(CFactor* factor);
int isSignedCType(CType* type);
void convertValFromType(uint64_t* val, CType* toType);
initialValueStaticInitType convertSpecTypeToStaticInitType(CType* type);
int ctypeEqual(CType* a, CType* b);
int isNullPointerConstant(CFactor* factor);
int isLvalue(const CFactor* expr);
int isArithmeticType(CType* type);
int getFunctionParamCount(const CType* funcType);
inline int getFunctionParamCount(const CType* funcType);
CFactor* convertTo(CFactor* expr, CType* targetType);
CFactor* convertByAssignment(CFactor* expr, CType* targetType);