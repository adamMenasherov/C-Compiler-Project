#include "SharedTypeRank.h"
#include "DataStructures/HashTable/Wrappers/SymbolTableWrapper.h"

#include <stdio.h>
#include <stdlib.h>

int size(specifierType type) {
    switch (type) {
        case SPEC_INT:
        case SPEC_UNSIGNED_INT:
            return 4;
        case SPEC_LONG:
        case SPEC_UNSIGNED_LONG:
            return 8;
        default:
            fprintf(stderr, "Invalid specifier type in size function\n");
            exit(1);
    }
}


int isSignedIdentifierType(IdentifierType type) {
    switch (type) {
        case TYPE_INT:
        case TYPE_LONG:
            return 1;
        case TYPE_UNSIGNED_INT:
        case TYPE_UNSIGNED_LONG:
            return 0;
        default:
            fprintf(stderr, "Invalid identifier type in isSignedIdentifierType function\n");
            exit(1);
    }
}

specifierType constantTypeToSpecifierType(constantType type) {
    switch (type) {
        case CONST_INT:
            return SPEC_INT;
        case CONST_LONG:
            return SPEC_LONG;
        case CONST_UNSIGNED_INT:
            return SPEC_UNSIGNED_INT;
        case CONST_UNSIGNED_LONG:
            return SPEC_UNSIGNED_LONG;
        case CONST_FLOATING_POINT:
            return SPEC_DOUBLE;
        default:
            fprintf(stderr, "Invalid constant type in constantTypeToSpecifierType function\n");
            exit(1);
    }
}

TACKYStaticVarType initialStaticTypeToTACKYStaticVarType(initialValueStaticInitType type) {
    switch (type) {
        case STATIC_INIT_INT:
        case STATIC_INIT_UNSIGNED_INT:
            return TACKY_INT;
        case STATIC_INIT_LONG:
        case STATIC_INIT_UNSIGNED_LONG:
            return TACKY_LONG;
        case STATIC_INIT_DOUBLE:
            return TACKY_DOUBLE;
        default:
            fprintf(stderr, "Invalid initial value static init type in initialStaticTypeToTACKYStaticVarType function\n");
            exit(1);
    }
}