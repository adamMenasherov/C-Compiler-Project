#include "SymbolParser.h"
#include "../../TACKYUtils/TACKYConstructors.h"

typedef void (*StaticVarHandler)(IdentifierTypeInfo* symbol, TACKYTopLevelArray* topLevels);

static initialValueStaticInitType identifierTypeToStaticInitType(IdentifierType type) {
    switch (type) {
        case TYPE_INT:
            return STATIC_INIT_INT;
        case TYPE_LONG:
            return STATIC_INIT_LONG;
        case TYPE_UNSIGNED_INT:
            return STATIC_INIT_UNSIGNED_INT;
        case TYPE_UNSIGNED_LONG:
            return STATIC_INIT_UNSIGNED_LONG;
        default:
            return STATIC_INIT_INT;
    }
}

static void handleInitialWithValue(IdentifierTypeInfo* symbol, TACKYTopLevelArray* topLevels) {
    double doubleVal;
    uint64_t intVal;
    if (symbol->attrs->attrs.staticAttr.initValue->value.staticInitVal.staticInitType == STATIC_INIT_DOUBLE) {
        doubleVal = symbol->attrs->attrs.staticAttr.initValue->value.staticInitVal.val.doubleVal;
    }
    else {
        intVal = symbol->attrs->attrs.staticAttr.initValue->value.staticInitVal.val.intVal;
    }
    
    initialValueStaticInitType staticInitType = symbol->attrs->attrs.staticAttr.initValue->value.staticInitVal.staticInitType;
    TACKYTopLevelArray_append(
        topLevels,
        createTACKYTopLevelFromStaticVar(createTACKYStaticVar(symbol->identifier, symbol->attrs->global, intVal, doubleVal, staticInitType))
    );
}

static void handleInitialTentative(IdentifierTypeInfo* symbol, TACKYTopLevelArray* topLevels) {
    initialValueStaticInitType staticInitType = identifierTypeToStaticInitType(symbol->type);
    TACKYTopLevelArray_append(
        topLevels,
        createTACKYTopLevelFromStaticVar(createTACKYStaticVar(symbol->identifier, symbol->attrs->global, 0, 0.0, staticInitType))
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