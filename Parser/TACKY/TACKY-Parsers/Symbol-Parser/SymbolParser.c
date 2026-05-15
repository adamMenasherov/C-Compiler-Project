#include "SymbolParser.h"
#include "../../TACKYUtils/TACKYConstructors.h"

typedef void (*StaticVarHandler)(IdentifierTypeInfo* symbol, TACKYTopLevelArray* topLevels);

static void handleInitialWithValue(IdentifierTypeInfo* symbol, TACKYTopLevelArray* topLevels) {
    int val = symbol->attrs->attrs.staticAttr.initValue->value.intValue;
    TACKYTopLevelArray_append(
        topLevels,
        createTACKYTopLevelFromStaticVar(createTACKYStaticVar(symbol->identifier, symbol->attrs->global, val))
    );
}

static void handleInitialTentative(IdentifierTypeInfo* symbol, TACKYTopLevelArray* topLevels) {
    TACKYTopLevelArray_append(
        topLevels,
        createTACKYTopLevelFromStaticVar(createTACKYStaticVar(symbol->identifier, symbol->attrs->global, 0))
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