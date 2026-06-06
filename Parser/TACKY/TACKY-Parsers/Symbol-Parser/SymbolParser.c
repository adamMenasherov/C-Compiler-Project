#include "SymbolParser.h"
#include "../../TACKYUtils/TACKYConstructors.h"

typedef void (*StaticVarHandler)(IdentifierTypeInfo* symbol, TACKYTopLevelArray* topLevels);

static initialValueStaticInitType ctypeToStaticInitType(CType* type) {
    if (!type) return STATIC_INIT_INT;
    switch (type->kind) {
        case CTYPE_INT:    return STATIC_INIT_INT;
        case CTYPE_LONG:   return STATIC_INIT_LONG;
        case CTYPE_UINT:   return STATIC_INIT_UNSIGNED_INT;
        case CTYPE_ULONG:  return STATIC_INIT_UNSIGNED_LONG;
        case CTYPE_POINTER:return STATIC_INIT_UNSIGNED_LONG;
        case CTYPE_DOUBLE: return STATIC_INIT_DOUBLE;
        default:           return STATIC_INIT_INT;
    }
}

static void handleInitialWithValue(IdentifierTypeInfo* symbol, TACKYTopLevelArray* topLevels) {
    initialValue* initVal = symbol->attrs->attrs.staticAttr.initValue;
    TACKYStaticVar* staticVar = createTACKYStaticVar(symbol->identifier, symbol->attrs->global, initVal->type);
    for (int i = 0; i < initVal->value.size; i++) {
        staticInitialVal* sv = initVal->value.staticInitVal[i];
        staticVar->initVal[staticVar->size++] = createStaticInitialVal(sv->staticInitType, sv->val.intVal, sv->val.doubleVal);
    }
    TACKYTopLevelArray_append(topLevels, createTACKYTopLevelFromStaticVar(staticVar));
}

static void handleInitialTentative(IdentifierTypeInfo* symbol, TACKYTopLevelArray* topLevels) {
    initialValueStaticInitType staticInitType = ctypeToStaticInitType(symbol->type);
    TACKYTopLevelArray_append(
        topLevels,
        createTACKYTopLevelFromStaticVar(createTACKYStaticVar(symbol->identifier, symbol->attrs->global, staticInitType))
    );
}

static const StaticVarHandler staticVarHandlers[] = {
    [INITIAL_WITH_VALUE] = handleInitialWithValue,
    [INITIAL_TENTATIVE] = handleInitialTentative,
};


void convertSymbolsToTACKY(IdentifierTypeInfo* symbol, void* userData) {
    TACKYTopLevelArray* topLevels = (TACKYTopLevelArray*)userData;
    if (!symbol || !topLevels) return;
    if (!symbol->attrs || symbol->attrs->attrType != IDENTIFIER_STATIC_ATTR || 
        !symbol->attrs->attrs.staticAttr.initValue || symbol->attrs->attrs.staticAttr.initValue->type == INITIAL_NO_VALUE) return;
    staticVarHandlers[symbol->attrs->attrs.staticAttr.initValue->type](symbol, topLevels);
}