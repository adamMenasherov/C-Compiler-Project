#include "SemanticUtils.h"
#include "Parser/generateUtils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* generateUniqueVariableName(char* baseName) {
    char* uniqueName = malloc(strlen(baseName) + 20);
    if (!uniqueName) return NULL;
    sprintf(uniqueName, "%s.%d", baseName, currGlobalInt++);
    return uniqueName;
}

const char* fromTempToOrigin(char* identifier) {
    return (const char*)strtok(identifier, ".");
}
specifierType identifierTypeToSpecifierType(IdentifierType type) {
    switch (type) {
        case TYPE_INT:
            return SPEC_INT;
        case TYPE_LONG:
            return SPEC_LONG;
        case TYPE_FUNCTION:
            return SPEC_NULL; // Functions don't have a specifier type in this context
        default:
            fprintf(stderr, "Invalid IdentifierType in identifierTypeToSpecifierType\n");
            exit(1);
    }
}

void setTypeVar(CFactor* expr, IdentifierType type) {
    expr->valueType = identifierTypeToSpecifierType(type);
}

void setType(CFactor* expr, specifierType type) {
    expr->valueType = type;
}

void setTypeConst(CFactor* expr, constantType type) {
    switch (type) {
        case CONST_INT:
            expr->valueType = SPEC_INT;
            break;
        case CONST_LONG:
            expr->valueType = SPEC_LONG;
            break;
        default:
            fprintf(stderr, "Invalid constant type in setTypeConst\n");
            exit(1);
    }
}

specifierType getCommonType(specifierType type1, specifierType type2) {
    if (type1 == type2) return type1;
    else return SPEC_LONG;
}

int isBasicType(specifierType type) {
    return type == SPEC_INT || type == SPEC_LONG;
}