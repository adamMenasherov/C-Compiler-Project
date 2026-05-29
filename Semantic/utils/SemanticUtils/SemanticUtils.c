#include "SemanticUtils.h"
#include "Parser/generateUtils.h"
#include "Parser/Common/SharedTypeRank.h"
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
        case TYPE_UNSIGNED_INT:
            return SPEC_UNSIGNED_INT;
        case TYPE_UNSIGNED_LONG:
            return SPEC_UNSIGNED_LONG;
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
        case CONST_UNSIGNED_INT:
            expr->valueType = SPEC_UNSIGNED_INT;
            break;
        case CONST_UNSIGNED_LONG:
            expr->valueType = SPEC_UNSIGNED_LONG;
            break;
        case CONST_FLOATING_POINT:
            expr->valueType = SPEC_DOUBLE;
            break;
        default:
            fprintf(stderr, "Invalid constant type in setTypeConst\n");
            exit(1);
    }
}

specifierType getCommonType(specifierType type1, specifierType type2) {
    if (type1 == type2) return type1;
    if (type1 == SPEC_DOUBLE || type2 == SPEC_DOUBLE) return SPEC_DOUBLE;
    if (size(type1) == size(type2)) {
        if (isSignedSpecifier(type1)) return type2;
        else return type1;
    }
    if (size(type1) > size(type2)) return type1;
    else return type2;
}

int isBasicType(specifierType type) {
    return type == SPEC_INT || type == SPEC_LONG || type == SPEC_UNSIGNED_INT || type == SPEC_UNSIGNED_LONG;
}

IdentifierType specifierTypeToIdentifierType(specifierType type) {
    switch (type) {
        case SPEC_INT:
            return TYPE_INT;
        case SPEC_LONG:
            return TYPE_LONG;
        case SPEC_UNSIGNED_INT:
            return TYPE_UNSIGNED_INT;
        case SPEC_UNSIGNED_LONG:
            return TYPE_UNSIGNED_LONG;
        case SPEC_DOUBLE:
            return TYPE_DOUBLE;
        default:
            fprintf(stderr, "Invalid specifier type in specifierTypeToIdentifierType\n");
            exit(1);
    }
}

initialValueStaticInitType convertExpTypeToStaticInitType(specifierType expType) {
    switch (expType) {
        case SPEC_INT: return STATIC_INIT_INT;
        case SPEC_LONG: return STATIC_INIT_LONG;
        default:
            fprintf(stderr, "Semantic Error: Unsupported initializer type for file scope variable\n");
            exit(1);
    }
}

specifierType getType(CFactor* factor) {
    return factor->valueType;
}

int isSignedSpecifier(specifierType type) {
    return type == SPEC_INT || type == SPEC_LONG;
}

void convertValFromType(uint64_t* val, specifierType toType) {
    switch (toType) {
        case SPEC_INT:
            *val = (int)(*val);
            return;
        case SPEC_LONG:
            *val = (long)(*val);
            return;
        case SPEC_UNSIGNED_INT:
            *val = (unsigned int)(*val);
            return;
        case SPEC_UNSIGNED_LONG:
            *val = (unsigned long)(*val);
            return;
        default:
            fprintf(stderr, "Invalid specifier type in convertValFromType\n");
            exit(1);
    }
}