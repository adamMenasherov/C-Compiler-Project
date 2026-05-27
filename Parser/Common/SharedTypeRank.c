#include "SharedTypeRank.h"

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
