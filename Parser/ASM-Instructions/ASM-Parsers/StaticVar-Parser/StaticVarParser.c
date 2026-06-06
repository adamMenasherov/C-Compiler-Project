#include "StaticVarParser.h"
#include <string.h>

ASMTopLevel* createASMStaticVarFromTACKYStaticVar(TACKYStaticVar* tackyStaticVar) {
    ASMStaticVar* asmStaticVar = malloc(sizeof(ASMStaticVar));
    if (!asmStaticVar) return NULL;

    asmStaticVar->identifier = strdup(tackyStaticVar->identifier);
    if (!asmStaticVar->identifier) {
        free(asmStaticVar);
        return NULL;
    }
    asmStaticVar->global = tackyStaticVar->global;
    memcpy(asmStaticVar->initVal, tackyStaticVar->initVal, sizeof(staticInitialVal*) * MAX_STATIC_SIZE);
    if (tackyStaticVar->initVal[0]->staticInitType == STATIC_INIT_INT
        || tackyStaticVar->initVal[0]->staticInitType == STATIC_INIT_UNSIGNED_INT) {
        asmStaticVar->alignment = 4;
    } else if (tackyStaticVar->initVal[0]->staticInitType == STATIC_INIT_LONG
               || tackyStaticVar->initVal[0]->staticInitType == STATIC_INIT_UNSIGNED_LONG) {
        asmStaticVar->alignment = 8;
    } else {
        asmStaticVar->alignment = 4;
    }

    return createTopLevel(ASM_TOP_LEVEL_STATIC_VAR, asmStaticVar);
}
