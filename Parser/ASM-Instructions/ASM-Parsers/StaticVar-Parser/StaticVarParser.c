#include "StaticVarParser.h"

ASMTopLevel* createASMStaticVarFromTACKYStaticVar(TACKYStaticVar* tackyStaticVar) {
    ASMStaticVar* asmStaticVar = malloc(sizeof(ASMStaticVar));
    if (!asmStaticVar) return NULL;

    asmStaticVar->identifier = strdup(tackyStaticVar->identifier);
    if (!asmStaticVar->identifier) {
        free(asmStaticVar);
        return NULL;
    }
    asmStaticVar->global = tackyStaticVar->global;
    asmStaticVar->initVal.val = tackyStaticVar->initVal.val;
    asmStaticVar->initVal.staticInitType = tackyStaticVar->initVal.staticInitType;
    if (tackyStaticVar->initVal.staticInitType == STATIC_INIT_INT
        || tackyStaticVar->initVal.staticInitType == STATIC_INIT_UNSIGNED_INT) {
        asmStaticVar->alignment = 4;
    } else if (tackyStaticVar->initVal.staticInitType == STATIC_INIT_LONG
               || tackyStaticVar->initVal.staticInitType == STATIC_INIT_UNSIGNED_LONG) {
        asmStaticVar->alignment = 8;
    } else {
        asmStaticVar->alignment = 4;
    }

    return createTopLevel(ASM_TOP_LEVEL_STATIC_VAR, asmStaticVar);
}
